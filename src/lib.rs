extern crate libc;

use std::ffi::CStr;
use std::mem;

// [Video] Refresh Fn
//  - buffer: *const u8
//  - width: u32
//  - height: u32
//  - pitch: u32
pub type VideoRefreshFn = extern "C" fn(*const u8, u32, u32, u32) -> ();

// [Input] Kind
pub enum Input {
    Joypad,
    Keyboard,
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
pub type InputStateFn = extern "C" fn(u8, u8, u16) -> bool;

// Generic "Core" trait that must be implemented by a back-end
pub trait Core {
    // Reset the environment as if it was turned off and then back on
    fn reset(&mut self);

    // Run for the next "frame"
    fn run_next(&mut self);

    // Insert ROM
    fn insert_rom(&mut self, filename: &str);

    // Remove ROM (if inserted)
    fn remove_rom(&mut self);

    // Store the `video_refresh` Fn
    //  - The back-end should invoke this when a "frame" of
    //    video is complete (generally during V-Blank of the guest machine)
    fn set_video_refresh(&mut self, cb: VideoRefreshFn);

    // Store the `input_state` Fn
    fn set_input_state(&mut self, cb: InputStateFn);
}

// Create a new (boxed) instance of a core
unsafe fn new<T: 'static + Core + Default>() -> Box<T> {
    Box::new(T::default())
}

// Generate extern methods to "expose" the core
#[macro_export]
macro_rules! ax_core (($t:path) => {
  extern crate libc;

  #[no_mangle]
  pub unsafe extern "C" fn ax_new() -> *mut Core {
      mem::transmute(new::<$t>())
  }
});

#[no_mangle]
pub unsafe extern "C" fn ax_delete(ptr: *mut Core) {
    let _drop_me: Box<Core> = mem::transmute(ptr);
}

#[no_mangle]
pub unsafe extern "C" fn ax_reset(ptr: *mut Core) {
    (*ptr).reset();
}

#[no_mangle]
pub unsafe extern "C" fn ax_set_video_refresh(ptr: *mut Core, cb: VideoRefreshFn) {
    (*ptr).set_video_refresh(cb);
}

#[no_mangle]
pub unsafe extern "C" fn ax_insert_rom(ptr: *mut Core, filename: *const libc::c_char) {
    let filename = CStr::from_ptr(filename).to_str().unwrap();

    (*ptr).insert_rom(filename);
}

#[no_mangle]
pub unsafe extern "C" fn ax_remove_rom(ptr: *mut Core) {
    (*ptr).remove_rom();
}

#[no_mangle]
pub unsafe extern "C" fn ax_run_next(ptr: *mut Core) {
    (*ptr).run_next();
}
