add_executable(gvrdp
    main.cpp

    # Utilities
    util/logger.cpp
    util/debouncer.cpp

    # Config
    config/connection_profile.cpp
    config/profile_store.cpp
    config/app_config.cpp

    # Core RDP
    core/rdp_session.cpp
    core/rdp_settings.cpp
    core/rdp_callbacks.cpp
    core/rdp_channels.cpp

    # Channels
    channels/disp_channel.cpp
    channels/cliprdr_channel.cpp
    channels/rdpsnd_channel.cpp
    channels/rdpdr_channel.cpp

    # Rendering
    render/sdl_renderer.cpp
    render/sdl_cursor.cpp

    # Input
    input/input_handler.cpp
    input/keyboard_map.cpp

    # UI
    ui/ui_manager.cpp
    ui/connection_dialog.cpp
    ui/settings_dialog.cpp
    ui/profile_manager_dialog.cpp
)

target_include_directories(gvrdp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gvrdp PRIVATE
    PkgConfig::FREERDP3
    PkgConfig::FREERDP_CLIENT3
    PkgConfig::WINPR3
    SDL2::SDL2
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    imgui_sdl2
    pthread
)

set_compiler_warnings(gvrdp)

# Copy assets to build directory
add_custom_command(TARGET gvrdp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:gvrdp>/assets
)
