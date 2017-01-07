extern crate libc;

use std::ffi::CStr;
use std::mem;

// [Video] Refresh Fn
//  - buffer: *const u8
//  - width: u32
//  - height: u32
//  - pitch: u32
#[doc(hidden)]
pub type UnsafeVideoRefreshFn = extern "C" fn(*const u8, u32, u32) -> ();

// [Input] Kind
enum Input {
    Joypad = 0,
    Keyboard = 1,
}

// [Input] Joypad
pub enum Joypad {
    A,
    B,
    X,
    Y,

    Select,
    Start,

    Up,
    Down,
    Left,
    Right,
}

// [Input] Keyboard
pub enum Keyboard {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    Num0,

    Backspace,
    Tab,
    Return,
    Escape,
    Space,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    PrintScreen,
    ScrollLock,
    Pause,

    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,

    Right,
    Left,
    Down,
    Up,
}

// [Input] State Fn
//  Get the current state of input. This is updated at most once per
//  frame.
//
//  - port: u8
//  - device: u8 (Input)
//  - id: u16 (Keyboard/Joypad)
#[doc(hidden)]
pub type UnsafeInputStateFn = extern "C" fn(u8, u8, u32) -> i16;

/// Pixel Formats available for software rendering.
pub enum PixelFormat {
    // 8-bit: 3-bits for Red and Green, 2-bits for Blue
    R3_G3_B2,

    // 16-bit: 5-bits for Red and Blue, 6-bits for Green
    R5_B5_G6,

    // 32-bit: 10-bits for Red, Green, and Blue (2 unused bits)
    R10_G10_B10,
}

// Runtime
#[derive(Default)]
pub struct Runtime {
    video_refresh_fn: Option<UnsafeVideoRefreshFn>,
    input_state_fn: Option<UnsafeInputStateFn>,
}

impl Runtime {
    /// Refreshes the video display (framebuffer).
    ///
    /// Cores that use this method generally contain a fully software renderer that
    /// outputs to a pixel framebuffer.
    ///
    /// Note that `width` and `height` may freely change frame-to-frame but
    /// the pixel format of `data` must not change.
    ///
    /// # Arguments
    ///
    /// * `data` — Immutable reference to a raw framebuffer containing pixel
    ///            data in the defined pixel format in the configuration.
    /// * `width` — Width of the display in pixels
    /// * `height` — Height of the display in pixels
    ///
    pub fn video_refresh(&self, data: &[u8], width: u32, height: u32) {
        if let Some(ref video_refresh_fn) = self.video_refresh_fn {
            (video_refresh_fn)(data.as_ptr(), width, height);
        }
    }

    /// Get the state of a key from a `Keyboard` device.
    pub fn input_keyboard_state(&self, port: u8, key: Keyboard) -> bool {
        if let Some(ref input_state_fn) = self.input_state_fn {
            (input_state_fn)(port, Input::Keyboard as u8, key as u32) == 1
        } else {
            false
        }
    }
}

// Bundle
//  Bundles the Runtime and Core into a handle that is managed by the front-end.
pub struct Bundle {
    runtime: Runtime,
    core: Box<Core>,
}

// Generic "Core" trait that must be implemented by a back-end
pub trait Core {
    /// Soft reset
    fn reset(&mut self);

    /// Run for the next "frame"
    fn run_next(&mut self, &mut Runtime);

    /// Insert ROM
    fn rom_insert(&mut self, filename: &str);

    /// Remove ROM (if inserted)
    ///
    /// The core should act as if the ROM was forcibly removed and continue
    /// operation (whatever consequences that may have). The front-end
    /// is generally expected to have stopped calling `run_next` before this.
    ///
    fn rom_remove(&mut self);
}

// Create a new (boxed) instance of a core
unsafe fn _new<T: 'static + Core + Default>() -> Box<Bundle> {
    Box::new(Bundle {
        core: Box::new(T::default()),
        runtime: Default::default(),
    })
}

// Generate extern methods to "expose" the core
#[macro_export]
macro_rules! ax_core (($t:path) => {
  extern crate libc;

  #[no_mangle]
  pub unsafe extern "C" fn ax_new() -> *mut Bundle {
      mem::transmute(_new::<$t>())
  }
});

#[no_mangle]
pub unsafe extern "C" fn ax_delete(ptr: *mut Bundle) {
    let _drop_me: Box<Bundle> = mem::transmute(ptr);
}

#[no_mangle]
pub unsafe extern "C" fn ax_reset(ptr: *mut Bundle) {
    (*ptr).core.reset();
}

#[no_mangle]
pub unsafe extern "C" fn ax_set_video_refresh(ptr: *mut Bundle, cb: UnsafeVideoRefreshFn) {
    (*ptr).runtime.video_refresh_fn = Some(cb);
}

#[no_mangle]
pub unsafe extern "C" fn ax_set_input_state(ptr: *mut Bundle, cb: UnsafeInputStateFn) {
    (*ptr).runtime.input_state_fn = Some(cb);
}

#[no_mangle]
pub unsafe extern "C" fn ax_rom_insert(ptr: *mut Bundle, filename: *const libc::c_char) {
    let filename = CStr::from_ptr(filename).to_str().unwrap();

    (*ptr).core.rom_insert(filename);
}

#[no_mangle]
pub unsafe extern "C" fn ax_rom_remove(ptr: *mut Bundle) {
    (*ptr).core.rom_remove();
}

#[no_mangle]
pub unsafe extern "C" fn ax_run_next(ptr: *mut Bundle) {
    (*ptr).core.run_next(&mut (*ptr).runtime);
}
