#include "core.h"

#include <chrono>
#include <cstdio>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

ax::Core::Core(const char* filename) : _lib(filename), _handle(nullptr), _running(false) {
  _handle = _lib.get<void*()>("ax_new")();
}

ax::Core::~Core() noexcept {
  // Only pauses if needed
  pause();

  if (_handle) {
    _lib.get<void(void*)>("ax_delete")(_handle);

    _handle = nullptr;
  }
}

void ax::Core::resume() {
  _running = true;
  _thread.reset(new std::thread(ax::Core::_main, this));
}

void ax::Core::pause() {
  if (_running) {
    _running = false;

    _thread->join();
    _thread = nullptr;
  }
}

void ax::Core::reset() {
  _lib.get<void(void*)>("ax_reset")(_handle);
}

void ax::Core::insert_rom(const char* filename) {
  // TODO: Propagate errors here
  _lib.get<void(void*, const char*)>("ax_insert_rom")(_handle, filename);
}

void ax::Core::remove_rom() {
  _lib.get<void(void*)>("ax_remove_rom")(_handle);
}

void ax::Core::set_video_refresh(VideoRefreshCallback callback) {
  _lib.get<void(void*, VideoRefreshCallback)>("ax_set_video_refresh")(_handle, callback);
}

void ax::Core::_main(Core* self) noexcept {
  auto run_next = self->_lib.get<void(void*)>("ax_run_next");
  auto handle = self->_handle;

  while (self->_running) {
    auto now = high_resolution_clock::now();

    // Run next "frame"
    run_next(handle);

    // Wait until end of frame
    // TODO: Configurable frame rate
    // Each frame should take: 16666 microseconds
    long long elapsed_us = 0;
    long long rem = 0;

    do {
      elapsed_us = duration_cast<microseconds>(high_resolution_clock::now() - now).count();
      rem = elapsed_us < 16666 ? (16666 - elapsed_us) : 0;

      if (rem > 1000) {
        std::this_thread::sleep_for(std::chrono::microseconds(rem));
      }
    } while (rem > 0);
  }
}
