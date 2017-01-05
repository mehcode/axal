#ifndef AXAL_CORE_H
#define AXAL_CORE_H 1

#include <memory>
#include <thread>

#include "library.h"

// TODO: Need some method to get a specific core by name
// TODO: Need some method to determine which core will handle a speicifc ROM

namespace ax {

using VideoRefreshCallback = void(*)(uint8_t*, uint32_t, uint32_t, uint32_t);

class Core {
 public:
  Core(const char* filename);

  virtual ~Core() noexcept;

  void resume();
  void pause();

  void reset();

  void insert_rom(const char* filename);
  void remove_rom();

  void set_video_refresh(VideoRefreshCallback callback);

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
