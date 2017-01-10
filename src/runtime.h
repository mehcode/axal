#ifndef AXAL_RUNTIME_H
#define AXAL_RUNTIME_H 1

#include <memory>
#include <thread>

#include "library.h"
#include "axal.h"

namespace ax {

class Runtime {
 public:
  Runtime();

  virtual ~Runtime() noexcept;

  void core_load(void* userdata, const char* filename);
  void core_unload();

  void rom_insert(const char* filename);
  void rom_remove();

  void reset();

  void pause();
  void resume();

  void set_video_refresh(axal_video_refresh_fn callback);

  bool is_loaded() const {
    return _core != nullptr;
  }

  bool is_running() const {
    return _running;
  }

 private:
  static void _main(Runtime* self) noexcept;
  static int16_t _input_state(void*, uint8_t, uint8_t, uint32_t) noexcept;

  // Handle to loaded library exposing core
  std::unique_ptr<Library> _lib;

  // Function table of core
  axal_core _core_vt;

  // Opaque handle of created core
  void* _core;

  // Thread where _main is running
  std::unique_ptr<std::thread> _thread;

  // Are we still running
  bool _running = false;

  // Video refresh callback (saved across cores)
  axal_video_refresh_fn _video_refresh_cb;

};

}  // namespace ax

#endif  // AXAL_RUNTIME_H
