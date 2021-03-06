extern crate libc;

use std::ffi::CStr;
use std::ffi::CString;
use std::mem;

mod sys {
    use libc;

    #[repr(C)]
    pub struct Info {
        pub library_name: *const libc::c_char,
        pub library_version: *const libc::c_char,
        pub pixel_format: u8,
        pub min_width: u32,
        pub min_height: u32,
        pub max_width: u32,
        pub max_height: u32,
    }
}

// [Video] Refresh Fn
//  - buffer: *const u8
//  - width: u32
//  - height: u32
//  - pitch: u32
#[doc(hidden)]
type UnsafeVideoRefreshFn = extern "C" fn(*mut libc::c_void, *const u8, u32, u32) -> ();

// [Input] Kind
enum Input {
    Keyboard = 0,
    Joypad = 1,
}

// [Input] Joypad
#[derive(Clone, Copy)]
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

// [Input] Key
#[derive(Clone, Copy)]
pub enum Key {
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
type UnsafeInputStateFn = extern "C" fn(*mut libc::c_void, u8, u8, u32) -> i16;

/// Pixel Formats available for software rendering.
#[derive(Clone, Copy)]
pub enum PixelFormat {
    // 8-bit: 3-bits for Red and Green, 2-bits for Blue
    R3_G3_B2,

    // 16-bit: 5-bits for Red and Blue, 6-bits for Green
    R5_B5_G6,

    // 24-bit: 8-bits for Red, Green, and Blue
    R8_G8_B8,

    // 32-bit: 10-bits for Red, Green, and Blue (2 unused bits)
    R10_G10_B10,
}

pub struct Info {
    /// Name of library providing the core
    library_name: CString,

    /// Version of library providing the core
    library_version: CString,

    pixel_format: PixelFormat,

    min_width: u32,
    min_height: u32,

    max_width: u32,
    max_height: u32,
}

impl Info {
    pub fn new(name: &str, version: &str) -> Self {
        // TODO: Handle unwrap with defaults perhaps?
        Info {
            library_name: CString::new(name).unwrap(),
            library_version: CString::new(version).unwrap(),

            pixel_format: PixelFormat::R5_B5_G6,

            min_width: 0,
            min_height: 0,

            max_width: 0,
            max_height: 0,
        }
    }

    pub fn pixel_format(self, pixel_format: PixelFormat) -> Self {
        Info { pixel_format: pixel_format, ..self }
    }

    pub fn size(self, width: u32, height: u32) -> Self {
        Info {
            min_width: width,
            min_height: height,

            max_width: if self.max_width == 0 {
                width
            } else {
                self.max_width
            },

            max_height: if self.max_height == 0 {
                height
            } else {
                self.max_height
            },

            ..self
        }
    }

    pub fn max_size(self, width: u32, height: u32) -> Self {
        Info {
            max_width: width,
            max_height: height,

            ..self
        }
    }
}

// Runtime
pub struct Runtime {
    video_refresh_fn: Option<UnsafeVideoRefreshFn>,
    input_state_fn: Option<UnsafeInputStateFn>,
    userdata: *mut libc::c_void,
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
            (video_refresh_fn)(self.userdata, data.as_ptr(), width, height);
        }
    }

    /// Get the state of a key from a `Keyboard` device.
    pub fn input_keyboard_state(&self, port: u8, key: Key) -> bool {
        if let Some(ref input_state_fn) = self.input_state_fn {
            (input_state_fn)(self.userdata, port, Input::Keyboard as u8, key as u32) == 1
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
    info: Info,
}

// Generic "Core" trait that must be implemented by a back-end
pub trait Core {
    /// Information
    fn info(&self) -> Info;

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

#[doc(hidden)]
pub unsafe fn new<T: 'static + Core + Default>(userdata: *mut libc::c_void) -> Box<Bundle> {
    let core = Box::new(T::default());

    Box::new(Bundle {
        info: core.info(),
        core: core,
        runtime: Runtime {
            userdata: userdata,
            video_refresh_fn: None,
            input_state_fn: None,
        },
    })
}

// Generate trampoline to run the bundled UI from a stand-alone executable
#[macro_export]
macro_rules! ax_generate_main (($lib:ident) => {
    #[link_args = "-export-dynamic"]
    extern "C" {}

    extern crate libc;

    extern crate $lib;

    #[link(name = "axal")]
    extern "C" {
        fn axal_main(argc: libc::c_int, argv: *const *const libc::c_schar) -> ();
    }

    fn main() {
        unsafe {
            // Run
            axal_main(0, std::ptr::null());

            // HACK: This forces a native link of the rlib so the
            //       C API symbols get re-exported here
            let _ = $lib::ax_new(std::ptr::null_mut());
        }
    }
});

// Generate extern methods to "expose" the core from a shared library
#[macro_export]
macro_rules! ax_generate_lib (($t:path) => {
    #[no_mangle]
    pub unsafe extern "C" fn ax_new(userdata: *mut u8) -> *mut $crate::Bundle {
        use std::mem;

        mem::transmute($crate::new::<$t>(mem::transmute(userdata)))
    }
});

#[no_mangle]
pub unsafe extern "C" fn ax_get_info(ptr: *mut Bundle, info: *mut sys::Info) {
    if info.is_null() {
        // TODO: Panic/Error?
        return;
    }

    (*info).library_name = (*ptr).info.library_name.as_ptr();
    (*info).library_version = (*ptr).info.library_version.as_ptr();
    (*info).pixel_format = (*ptr).info.pixel_format as u8;
    (*info).min_width = (*ptr).info.min_width;
    (*info).min_height = (*ptr).info.min_height;
    (*info).max_width = (*ptr).info.max_width;
    (*info).max_height = (*ptr).info.max_height;
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_delete(ptr: *mut Bundle) {
    let _drop_me: Box<Bundle> = mem::transmute(ptr);
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_reset(ptr: *mut Bundle) {
    (*ptr).core.reset();
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_set_video_refresh(ptr: *mut Bundle, cb: UnsafeVideoRefreshFn) {
    (*ptr).runtime.video_refresh_fn = Some(cb);
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_set_input_state(ptr: *mut Bundle, cb: UnsafeInputStateFn) {
    (*ptr).runtime.input_state_fn = Some(cb);
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_rom_insert(ptr: *mut Bundle, filename: *const libc::c_char) {
    let filename = CStr::from_ptr(filename).to_str().unwrap();

    (*ptr).core.rom_insert(filename);
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_rom_remove(ptr: *mut Bundle) {
    (*ptr).core.rom_remove();
}

#[doc(hidden)]
#[no_mangle]
pub unsafe extern "C" fn ax_run_next(ptr: *mut Bundle) {
    (*ptr).core.run_next(&mut (*ptr).runtime);
}
