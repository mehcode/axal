#include "runtime.h"

#include <SDL.h>

#include <chrono>
#include <cassert>
#include <cstdio>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

ax::Runtime::Runtime() : _lib(nullptr), _core(nullptr), _running(false), _video_refresh_cb(nullptr) {
  // Initialize SDL; using as the input controller
  SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
}

ax::Runtime::~Runtime() noexcept {
  // Destroy SDL
  SDL_Quit();

  // Pause (if needed)
  pause();

  // Destroy core (if needed)
  if (_core) {
    _core_vt.delete_(_core);
    _core = nullptr;
  }
}

void ax::Runtime::core_load(void* userdata, const char* filename) {
  _lib.reset(new Library(filename));

  // Populate function table
  _core_vt.new_ = _lib->get<void*(void*)>("ax_new");
  _core_vt.delete_ = _lib->get<void(void*)>("ax_delete");
  _core_vt.set_video_refresh = _lib->get<void(void*, axal_video_refresh_fn)>("ax_set_video_refresh");
  _core_vt.set_input_state = _lib->get<void(void*, axal_input_state_fn)>("ax_set_input_state");
  _core_vt.rom_insert = _lib->get<void(void*, const char*)>("ax_rom_insert");
  _core_vt.rom_remove = _lib->get<void(void*)>("ax_rom_remove");
  _core_vt.run_next = _lib->get<void(void*)>("ax_run_next");
  _core_vt.reset = _lib->get<void(void*)>("ax_reset");

  // Instantiate core
  _core = _core_vt.new_(userdata);

  // Configure callbacks
  _core_vt.set_video_refresh(_core, _video_refresh_cb);
  _core_vt.set_input_state(_core, _input_state);
}

void ax::Runtime::core_unload() {
  _lib.reset(NULL);
  std::memset(&_core_vt, 0, sizeof (_core_vt));
}

void ax::Runtime::resume() {
  if (!_core || !_lib) return;

  _running = true;
  _thread.reset(new std::thread(ax::Runtime::_main, this));
}

void ax::Runtime::pause() {
  if (!_core || !_lib) return;
  if (_running) {
    _running = false;

    _thread->join();
    _thread = nullptr;
  }
}

void ax::Runtime::reset() {
  _core_vt.reset(_core);
}

void ax::Runtime::rom_insert(const char* filename) {
  // TODO: Propagate errors here
  _core_vt.rom_insert(_core, filename);
}

void ax::Runtime::rom_remove() {
  _core_vt.rom_remove(_core);
}

void ax::Runtime::set_video_refresh(axal_video_refresh_fn callback) {
  _video_refresh_cb = callback;

  if (_core) {
    _core_vt.set_video_refresh(_core, _video_refresh_cb);
  }
}

int16_t ax::Runtime::_input_state(void*, uint8_t port, uint8_t device, uint32_t key) noexcept {
  // NOTE: Only keyboard#0 implemented right now
  assert(port == 0);
  assert(device == 0);

  // Query SDL for keyboard state
  auto state = SDL_GetKeyboardState(NULL);

  // Convert ax::key to SDL_Keysym
  // TODO: This probably converts into a jump table; but we should probably check
  SDL_Scancode code;
  switch (key) {
    case AXKEY_A: code = SDL_SCANCODE_A; break;
    case AXKEY_B: code = SDL_SCANCODE_B; break;
    case AXKEY_C: code = SDL_SCANCODE_C; break;
    case AXKEY_D: code = SDL_SCANCODE_D; break;
    case AXKEY_E: code = SDL_SCANCODE_E; break;
    case AXKEY_F: code = SDL_SCANCODE_F; break;
    case AXKEY_G: code = SDL_SCANCODE_G; break;
    case AXKEY_H: code = SDL_SCANCODE_H; break;
    case AXKEY_I: code = SDL_SCANCODE_I; break;
    case AXKEY_J: code = SDL_SCANCODE_J; break;
    case AXKEY_K: code = SDL_SCANCODE_K; break;
    case AXKEY_L: code = SDL_SCANCODE_L; break;
    case AXKEY_M: code = SDL_SCANCODE_M; break;
    case AXKEY_N: code = SDL_SCANCODE_N; break;
    case AXKEY_O: code = SDL_SCANCODE_O; break;
    case AXKEY_P: code = SDL_SCANCODE_P; break;
    case AXKEY_Q: code = SDL_SCANCODE_Q; break;
    case AXKEY_R: code = SDL_SCANCODE_R; break;
    case AXKEY_S: code = SDL_SCANCODE_S; break;
    case AXKEY_T: code = SDL_SCANCODE_T; break;
    case AXKEY_U: code = SDL_SCANCODE_U; break;
    case AXKEY_V: code = SDL_SCANCODE_V; break;
    case AXKEY_W: code = SDL_SCANCODE_W; break;
    case AXKEY_X: code = SDL_SCANCODE_X; break;
    case AXKEY_Y: code = SDL_SCANCODE_Y; break;
    case AXKEY_Z: code = SDL_SCANCODE_Z; break;

    case AXKEY_NUM1: code = SDL_SCANCODE_1; break;
    case AXKEY_NUM2: code = SDL_SCANCODE_2; break;
    case AXKEY_NUM3: code = SDL_SCANCODE_3; break;
    case AXKEY_NUM4: code = SDL_SCANCODE_4; break;
    case AXKEY_NUM5: code = SDL_SCANCODE_5; break;
    case AXKEY_NUM6: code = SDL_SCANCODE_6; break;
    case AXKEY_NUM7: code = SDL_SCANCODE_7; break;
    case AXKEY_NUM8: code = SDL_SCANCODE_8; break;
    case AXKEY_NUM9: code = SDL_SCANCODE_9; break;
    case AXKEY_NUM0: code = SDL_SCANCODE_0; break;

    case AXKEY_BACKSPACE: code = SDL_SCANCODE_BACKSPACE; break;
    case AXKEY_TAB: code = SDL_SCANCODE_TAB; break;
    case AXKEY_RETURN: code = SDL_SCANCODE_RETURN; break;
    case AXKEY_ESCAPE: code = SDL_SCANCODE_ESCAPE; break;
    case AXKEY_SPACE: code = SDL_SCANCODE_SPACE; break;

    case AXKEY_F1: code = SDL_SCANCODE_F1; break;
    case AXKEY_F2: code = SDL_SCANCODE_F2; break;
    case AXKEY_F3: code = SDL_SCANCODE_F3; break;
    case AXKEY_F4: code = SDL_SCANCODE_F4; break;
    case AXKEY_F5: code = SDL_SCANCODE_F5; break;
    case AXKEY_F6: code = SDL_SCANCODE_F6; break;
    case AXKEY_F7: code = SDL_SCANCODE_F7; break;
    case AXKEY_F8: code = SDL_SCANCODE_F8; break;
    case AXKEY_F9: code = SDL_SCANCODE_F9; break;
    case AXKEY_F10: code = SDL_SCANCODE_F10; break;
    case AXKEY_F11: code = SDL_SCANCODE_F11; break;
    case AXKEY_F12: code = SDL_SCANCODE_F12; break;

    case AXKEY_PRINTSCREEN: code = SDL_SCANCODE_PRINTSCREEN; break;
    case AXKEY_SCROLLLOCK: code = SDL_SCANCODE_SCROLLLOCK; break;
    case AXKEY_PAUSE: code = SDL_SCANCODE_PAUSE; break;

    case AXKEY_INSERT: code = SDL_SCANCODE_INSERT; break;
    case AXKEY_HOME: code = SDL_SCANCODE_HOME; break;
    case AXKEY_PAGEUP: code = SDL_SCANCODE_PAGEUP; break;
    case AXKEY_DELETE: code = SDL_SCANCODE_DELETE; break;
    case AXKEY_END: code = SDL_SCANCODE_END; break;
    case AXKEY_PAGEDOWN: code = SDL_SCANCODE_PAGEDOWN; break;

    case AXKEY_RIGHT: code = SDL_SCANCODE_RIGHT; break;
    case AXKEY_LEFT: code = SDL_SCANCODE_LEFT; break;
    case AXKEY_DOWN: code = SDL_SCANCODE_DOWN; break;
    case AXKEY_UP: code = SDL_SCANCODE_UP; break;

    default:
      // Unhandled key
      return 0;
  }

  return state[code] == 1 ? 1 : 0;
}

void ax::Runtime::_main(Runtime* self) noexcept {
  while (self->_running) {
    auto now = high_resolution_clock::now();

    // Pump events to prepare to read input in this frame
    SDL_PumpEvents();

    // Run next "frame"
    self->_core_vt.run_next(self->_core);

    // Wait until end of frame
    // TODO: Configurable frame rate
    // Each frame should take: 16666 microseconds
    long long elapsed_us = 0;
    long long rem = 0;

    do {
      elapsed_us = duration_cast<microseconds>(high_resolution_clock::now() - now).count();
      rem = elapsed_us < 16666 ? (16666 - elapsed_us) : 0;

      // OS sleeps is not very accurate <1ms so only sleep until the last 1ms.
      if (rem > 1000) {
        std::this_thread::sleep_for(std::chrono::microseconds(rem - 1000));
      }
    } while (rem > 0);
  }
}
