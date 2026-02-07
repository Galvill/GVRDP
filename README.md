# GVRDP

Cross-platform RDP client with dynamic resolution adaptation. Resizing the window updates the remote session resolution seamlessly via the MS-RDPEDISP Display Control Virtual Channel — no reconnection required.

Built with FreeRDP 3, SDL2, Dear ImGui, spdlog, and nlohmann/json.

## Features

- **Dynamic resolution** — drag-resize the window and the remote desktop adapts after a 200ms debounce
- **Dear ImGui UI** — connection dialog with profile save/load, in-session overlay (Ctrl+Shift+S)
- **Clipboard sync** — copy/paste text between local and remote (CF_UNICODETEXT)
- **Full keyboard/mouse** — complete PS/2 scancode mapping including extended keys, mouse wheel, horizontal scroll
- **Profile management** — save connection profiles to disk (passwords never persisted)
- **Cross-platform** — targets Linux, Windows (vcpkg), and macOS

## Quickstart

### Prerequisites

**Linux (Ubuntu/Debian)**

```bash
sudo apt install build-essential cmake \
  freerdp3-dev libfreerdp-client3-dev libwinpr3-dev \
  libsdl2-dev libspdlog-dev nlohmann-json3-dev pkg-config
```

**macOS**

```bash
brew install freerdp sdl2 spdlog nlohmann-json cmake pkg-config
```

**Windows**

Install dependencies via [vcpkg](https://vcpkg.io). Set `VCPKG_ROOT` and use the `windows-debug` or `windows-release` preset.

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

Or use a preset:

```bash
cmake --preset linux-debug
cmake --build build/linux-debug -j$(nproc)
```

### Run

```bash
./build/src/gvrdp
```

A connection dialog appears. Enter the hostname, username, and password, then click **Connect**.

### Run Tests

```bash
cd build && ctest --output-on-failure
```

GTest is fetched automatically via CMake FetchContent if not installed on the system.

## Usage

| Action | Effect |
|--------|--------|
| Launch the app | Connection dialog appears |
| Fill in fields and click Connect | Initiates RDP connection |
| Drag-resize the window | Remote resolution updates after 200ms |
| Ctrl+Shift+S | Toggle in-session settings overlay |
| Disconnect button (in overlay) | Returns to connection dialog |

## Architecture

Two threads with a clean separation boundary:

```
MAIN THREAD                              RDP THREAD
┌──────────────────────┐                ┌──────────────────────┐
│ SDL event loop       │                │ freerdp_connect()    │
│ ImGui rendering      │  SDL_UserEvent │ WaitForMultipleObj   │
│ SDL_Texture updates  │ <───────────── │ check_event_handles  │
│ Input forwarding ────│───────────────>│ BeginPaint/EndPaint  │
│ Debouncer polling    │  send_mutex_   │ Channel callbacks    │
└──────────────────────┘                └──────────────────────┘
```

- **RDP → Main:** `EndPaint` pushes `SDL_UserEvent` with `GVRDP_EVENT_FRAME_READY`; main thread copies GDI buffer to SDL texture.
- **Main → RDP:** `freerdp_input_send_*` calls guarded by `send_mutex_`.
- **DISP channel:** Debouncer fires after 200ms quiet period, sends `DISPLAY_CONTROL_MONITOR_LAYOUT` via DVC.

## Project Structure

```
src/
├── main.cpp                 # Entry point, SDL event loop
├── core/                    # FreeRDP wrapper (context, session, callbacks)
├── channels/                # DISP, clipboard, audio, drive redirection
├── render/                  # SDL2 window, renderer, texture, cursor
├── input/                   # SDL → RDP input translation, scancode map
├── ui/                      # Dear ImGui dialogs and state machine
├── config/                  # Connection profiles, app config, JSON persistence
└── util/                    # Logger, debouncer, thread-safe queue, platform
tests/
├── test_connection_profile.cpp
├── test_debouncer.cpp
└── test_keyboard_map.cpp
```

## License

Unlicensed. Add your preferred license here.
