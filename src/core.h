#ifndef AXAL_CORE_H
#define AXAL_CORE_H 1

#include <memory>
#include <thread>

#include "library.h"

// TODO: Need some method to get a specific core by name
// TODO: Need some method to determine which core will handle a speicifc ROM

namespace ax {

using VideoRefreshFn = void(*)(uint8_t*, uint32_t, uint32_t, uint32_t);

using InputStateFn = void(*)(uint8_t, uint8_t, uint32_t);

class Core {
 public:
  Core(const char* filename);

  virtual ~Core() noexcept;

  void resume();
  void pause();

  void reset();

  void rom_insert(const char* filename);
  void rom_remove();

  void set_video_refresh(VideoRefreshFn callback);

  void set_input_state(InputStateFn callback);

  bool is_running() const {
    return _running;
  }

 private:
  static void _main(Core* self) noexcept;

  // Loaded Core module (shared library)
  Library _lib;

  // Opaque handle to Core
  void* _handle;

  // Thread where _main is running
  std::unique_ptr<std::thread> _thread;

  // Are we still running
  bool _running = false;

};

}  // namespace ax

#endif  // AXAL_CORE_H
