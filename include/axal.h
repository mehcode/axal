#ifndef AXAL_H
#define AXAL_H 1

#include <stdint.h>

// Video Refresh
typedef void (*axal_video_refresh_fn)(void*, uint8_t*, uint32_t, uint32_t, uint32_t);

// Pixel Format
enum axal_format: uint8_t {
  // 8-bit: 3-bits for Red and Green, 2-bits for Blue
  AXFORMAT_R3_G3_B2,

  // 16-bit: 5-bits for Red and Blue, 6-bits for Green
  AXFORMAT_R5_B5_G6,

  // 24-bit: 8-bits for Red, Green, and Blue
  AXFORMAT_R8_G8_B8,

  // 32-bit: 10-bits for Red, Green, and Blue (2 unused bits)
  AXFORMAT_R10_G10_B10,
};

// Core Information
typedef struct axal_info {
  const char* library_name;
  const char* library_version;
  axal_format pixel_format;
  uint32_t min_width;
  uint32_t min_height;
  uint32_t max_width;
  uint32_t max_height;
} axal_info;

// Input State
typedef int16_t (*axal_input_state_fn)(void*, uint8_t, uint8_t, uint32_t);

// Key enumeration (for keyboard)
enum axal_key {
  AXKEY_A,
  AXKEY_B,
  AXKEY_C,
  AXKEY_D,
  AXKEY_E,
  AXKEY_F,
  AXKEY_G,
  AXKEY_H,
  AXKEY_I,
  AXKEY_J,
  AXKEY_K,
  AXKEY_L,
  AXKEY_M,
  AXKEY_N,
  AXKEY_O,
  AXKEY_P,
  AXKEY_Q,
  AXKEY_R,
  AXKEY_S,
  AXKEY_T,
  AXKEY_U,
  AXKEY_V,
  AXKEY_W,
  AXKEY_X,
  AXKEY_Y,
  AXKEY_Z,

  AXKEY_NUM1,
  AXKEY_NUM2,
  AXKEY_NUM3,
  AXKEY_NUM4,
  AXKEY_NUM5,
  AXKEY_NUM6,
  AXKEY_NUM7,
  AXKEY_NUM8,
  AXKEY_NUM9,
  AXKEY_NUM0,

  AXKEY_BACKSPACE,
  AXKEY_TAB,
  AXKEY_RETURN,
  AXKEY_ESCAPE,
  AXKEY_SPACE,

  AXKEY_F1,
  AXKEY_F2,
  AXKEY_F3,
  AXKEY_F4,
  AXKEY_F5,
  AXKEY_F6,
  AXKEY_F7,
  AXKEY_F8,
  AXKEY_F9,
  AXKEY_F10,
  AXKEY_F11,
  AXKEY_F12,

  AXKEY_PRINTSCREEN,
  AXKEY_SCROLLLOCK,
  AXKEY_PAUSE,

  AXKEY_INSERT,
  AXKEY_HOME,
  AXKEY_PAGEUP,
  AXKEY_DELETE,
  AXKEY_END,
  AXKEY_PAGEDOWN,

  AXKEY_RIGHT,
  AXKEY_LEFT,
  AXKEY_DOWN,
  AXKEY_UP,
};

struct axal_core {
  // Create a new instance of the core.
  void* (*new_)(void*);

  // Information
  void (*get_info)(void*, axal_info*);

  // Destroy an instance and release all memory.
  void (*delete_)(void*);

  // Reset
  void (*reset)(void*);

  // Callbacks
  void (*set_video_refresh)(void*, axal_video_refresh_fn);
  void (*set_input_state)(void*, axal_input_state_fn);

  // Insert ROM
  void (*rom_insert)(void*, const char*);

  // Remove ROM
  void (*rom_remove)(void*);

  // Run Next Frame
  void (*run_next)(void*);
};

#endif  // AXAL_H
