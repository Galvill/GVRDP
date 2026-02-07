cmake_minimum_required(VERSION 3.22)
project(gvrdp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Custom CMake modules
include(cmake/CompilerWarnings.cmake)
include(cmake/Platform.cmake)

# ── Find system dependencies ──────────────────────────────────────────
find_package(PkgConfig REQUIRED)

# FreeRDP 3
pkg_check_modules(FREERDP3 REQUIRED IMPORTED_TARGET freerdp3)
pkg_check_modules(FREERDP_CLIENT3 REQUIRED IMPORTED_TARGET freerdp-client3)
pkg_check_modules(WINPR3 REQUIRED IMPORTED_TARGET winpr3)

# SDL2
find_package(SDL2 REQUIRED)

# spdlog
find_package(spdlog REQUIRED)

# nlohmann/json
find_package(nlohmann_json REQUIRED)

# ── FetchContent for Dear ImGui ───────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# Build Dear ImGui as a static library with SDL2 + SDLRenderer2 backends
add_library(imgui_sdl2 STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
target_include_directories(imgui_sdl2 PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_sdl2 PUBLIC SDL2::SDL2)
set_target_properties(imgui_sdl2 PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ── Subdirectories ────────────────────────────────────────────────────
add_subdirectory(src)

# ── Tests ─────────────────────────────────────────────────────────────
option(GVRDP_BUILD_TESTS "Build tests" ON)
if(GVRDP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
