#include "ui/ui_manager.hpp"

#include "render/sdl_renderer.hpp"
#include "ui/connection_dialog.hpp"
#include "ui/profile_manager_dialog.hpp"
#include "ui/settings_dialog.hpp"
#include "util/logger.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

namespace gvrdp {

UiManager::UiManager() = default;

UiManager::~UiManager() {
    shutdown();
}

bool UiManager::init(SdlRenderer& renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Slightly customize style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.FramePadding = ImVec2(8, 4);
    style.WindowPadding = ImVec2(12, 12);

    if (!ImGui_ImplSDL2_InitForSDLRenderer(renderer.window(), renderer.renderer())) {
        LOG_ERROR("ImGui_ImplSDL2_InitForSDLRenderer failed");
        return false;
    }

    if (!ImGui_ImplSDLRenderer2_Init(renderer.renderer())) {
        LOG_ERROR("ImGui_ImplSDLRenderer2_Init failed");
        return false;
    }

    imgui_initialized_ = true;
    LOG_INFO("ImGui initialized");
    return true;
}

void UiManager::shutdown() {
    if (imgui_initialized_) {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        imgui_initialized_ = false;
    }
}

bool UiManager::process_event(const SDL_Event& event) {
    if (!imgui_initialized_) return false;
    ImGui_ImplSDL2_ProcessEvent(&event);

    ImGuiIO& io = ImGui::GetIO();

    // Check if ImGui wants to capture the event
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT) {
        return io.WantCaptureKeyboard;
    }
    if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN ||
        event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        return io.WantCaptureMouse;
    }

    return false;
}

void UiManager::render(SdlRenderer& renderer) {
    if (!imgui_initialized_) return;

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    switch (state_) {
        case UiState::ConnectionDialog:
            draw_connection_dialog(current_profile_, on_connect_);
            break;

        case UiState::Connecting: {
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(300, 100));
            ImGui::Begin("Connecting", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoMove);
            ImGui::Text("Connecting to %s:%d...", current_profile_.hostname.c_str(),
                        current_profile_.port);
            ImGui::Text("Please wait...");
            if (ImGui::Button("Cancel", ImVec2(-1, 0))) {
                if (on_disconnect_) on_disconnect_();
            }
            ImGui::End();
            break;
        }

        case UiState::Connected:
            // No UI overlay — desktop only
            break;

        case UiState::OverlayVisible:
            draw_settings_dialog(current_profile_, on_disconnect_);
            break;

        case UiState::ErrorDialog: {
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(400, 150));
            ImGui::Begin("Error", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoMove);
            ImGui::TextWrapped("%s", error_message_.c_str());
            ImGui::Spacing();
            if (ImGui::Button("OK", ImVec2(-1, 0))) {
                state_ = UiState::ConnectionDialog;
            }
            ImGui::End();
            break;
        }
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer.renderer());
}

void UiManager::set_state(UiState state) {
    state_ = state;
}

void UiManager::show_error(const std::string& message) {
    error_message_ = message;
    state_ = UiState::ErrorDialog;
}

void UiManager::set_connecting() {
    state_ = UiState::Connecting;
}

void UiManager::set_connected() {
    state_ = UiState::Connected;
}

void UiManager::set_disconnected() {
    state_ = UiState::ConnectionDialog;
}

bool UiManager::check_overlay_toggle(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s &&
        (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.mod & KMOD_SHIFT)) {
        if (state_ == UiState::Connected) {
            state_ = UiState::OverlayVisible;
            return true;
        } else if (state_ == UiState::OverlayVisible) {
            state_ = UiState::Connected;
            return true;
        }
    }
    return false;
}

}  // namespace gvrdp
