#include "config/app_config.hpp"
#include "config/connection_profile.hpp"
#include "config/profile_store.hpp"
#include "core/rdp_session.hpp"
#include "input/input_handler.hpp"
#include "render/sdl_renderer.hpp"
#include "ui/ui_manager.hpp"
#include "util/debouncer.hpp"
#include "util/logger.hpp"
#include "util/platform.hpp"

#include <SDL2/SDL.h>

#include <chrono>
#include <memory>

using namespace gvrdp;

int main(int /*argc*/, char* /*argv*/[]) {
    // Initialize logging
    Logger::init("debug");
    LOG_INFO("GVRDP starting");

    // Load app config
    auto config_dir = get_config_dir();
    AppConfig app_config = AppConfig::load(config_dir);
    ProfileStore profile_store(config_dir);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        LOG_CRITICAL("SDL_Init failed: {}", SDL_GetError());
        return 1;
    }

    // Create renderer
    SdlRenderer renderer;
    if (!renderer.init("GVRDP - Remote Desktop", app_config.window_x, app_config.window_y,
                       app_config.window_w, app_config.window_h)) {
        LOG_CRITICAL("Failed to initialize renderer");
        SDL_Quit();
        return 1;
    }

    // Initialize UI
    UiManager ui;
    if (!ui.init(renderer)) {
        LOG_CRITICAL("Failed to initialize UI");
        SDL_Quit();
        return 1;
    }

    // RDP session and input handler (created on connect)
    std::unique_ptr<RdpSession> session;
    std::unique_ptr<InputHandler> input_handler;
    std::unique_ptr<Debouncer> resize_debouncer;

    // Load profiles
    auto profiles = profile_store.load_all();

    // Set up UI callbacks
    ui.set_connect_callback([&](const ConnectionProfile& profile) {
        LOG_INFO("Connecting to {}:{}", profile.hostname, profile.port);
        ui.set_connecting();

        session = std::make_unique<RdpSession>();
        input_handler = std::make_unique<InputHandler>(*session);

        // Create debouncer for resize events (200ms quiet period)
        resize_debouncer = std::make_unique<Debouncer>(
            std::chrono::milliseconds(200), [&]() {
                if (session && session->is_connected() && input_handler) {
                    uint32_t w = input_handler->pending_width();
                    uint32_t h = input_handler->pending_height();
                    if (w > 0 && h > 0) {
                        LOG_INFO("Debounced resize: {}x{}", w, h);
                        session->request_resolution_change(w, h);
                        input_handler->clear_pending_resize();
                    }
                }
            });

        if (!session->connect(profile, renderer.window_id())) {
            ui.show_error("Failed to connect: " + rdp_error_to_string(session->last_error()));
            session.reset();
            input_handler.reset();
            resize_debouncer.reset();
        }
    });

    ui.set_disconnect_callback([&]() {
        LOG_INFO("Disconnecting");
        if (session) {
            session->disconnect();
            session.reset();
            input_handler.reset();
            resize_debouncer.reset();
        }
        ui.set_disconnected();
    });

    // Main event loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Quit
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            // Check overlay toggle (Ctrl+Shift+S)
            if (ui.check_overlay_toggle(event)) {
                continue;
            }

            // Let ImGui process events first
            bool imgui_consumed = ui.process_event(event);

            // Handle SDL user events from RDP thread
            if (event.type == SDL_USEREVENT) {
                auto event_type = static_cast<GvrdpEvent>(event.user.code);
                switch (event_type) {
                    case GVRDP_EVENT_FRAME_READY:
                        // Frame data ready — will be copied below
                        if (session && session->is_connected() &&
                            ui.state() == UiState::Connecting) {
                            ui.set_connected();
                        }
                        break;

                    case GVRDP_EVENT_DISCONNECT:
                        LOG_INFO("RDP session disconnected");
                        session.reset();
                        input_handler.reset();
                        resize_debouncer.reset();
                        ui.set_disconnected();
                        break;

                    case GVRDP_EVENT_RESIZE:
                        // Server-side resize — recreate texture
                        if (session) {
                            renderer.resize_texture(session->gdi_width(), session->gdi_height());
                        }
                        break;

                    case GVRDP_EVENT_ERROR:
                        if (session) {
                            ui.show_error(
                                "Connection error: " +
                                rdp_error_to_string(session->last_error()));
                            session.reset();
                            input_handler.reset();
                            resize_debouncer.reset();
                        }
                        break;
                }
                continue;
            }

            // Forward input events to RDP if not consumed by ImGui
            if (!imgui_consumed && input_handler && session && session->is_connected()) {
                input_handler->handle_event(event);
            }
        }

        // Poll resize debouncer
        if (resize_debouncer) {
            if (input_handler && input_handler->has_pending_resize()) {
                resize_debouncer->trigger();
            }
            resize_debouncer->poll();
        }

        // Render frame
        renderer.clear();

        // Copy RDP desktop frame to texture
        if (session && session->is_connected()) {
            const uint8_t* buffer = session->gdi_buffer();
            uint32_t w = session->gdi_width();
            uint32_t h = session->gdi_height();
            uint32_t stride = session->gdi_stride();
            if (buffer && w > 0 && h > 0) {
                renderer.update_frame(buffer, w, h, stride);
            }
            renderer.render_desktop();
        }

        // Render ImGui UI on top
        ui.render(renderer);

        // Present
        renderer.present();
    }

    // Cleanup
    LOG_INFO("Shutting down");

    if (session) {
        session->disconnect();
        session.reset();
    }

    // Save window position/size
    SDL_GetWindowPosition(renderer.window(), &app_config.window_x, &app_config.window_y);
    app_config.window_w = renderer.window_width();
    app_config.window_h = renderer.window_height();
    app_config.save(config_dir);

    ui.shutdown();
    renderer.shutdown();
    SDL_Quit();

    LOG_INFO("GVRDP exited cleanly");
    return 0;
}
