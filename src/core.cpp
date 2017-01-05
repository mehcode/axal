#include "core.h"

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
    // Run next "frame"
    run_next(handle);

    // TODO: Wait until end of frame
  }
}
