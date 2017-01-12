#include "runtime.h"
#include "viewport.h"

#include <chrono>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

ax::Runtime::Runtime() : _lib(nullptr), _core(nullptr), _running(false), _video_refresh_cb(nullptr) {
}

ax::Runtime::~Runtime() noexcept {
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

int16_t ax::Runtime::_input_state(void* userdata, uint8_t port, uint8_t device, uint32_t key) noexcept {
  ax::Viewport* viewport = (ax::Viewport*)userdata;

  // NOTE: Only keyboard#0 implemented right now
  assert(port == 0);
  assert(device == 0);

  // TODO: Use cloned key state

  return viewport->_key_state[(axal_key)key] ? 1 : 0;
}

void ax::Runtime::_main(Runtime* self) noexcept {
  while (self->_running) {
    auto now = high_resolution_clock::now();

    // TODO: Clone key state

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
