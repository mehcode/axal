
// Video Refresh Callback
//  - buffer: *const u8
//  - width: u32
//  - height: u32
//  - pitch: u32
pub type VideoRefresh = extern "C" fn(*const u8, u32, u32, u32) -> ();

// Generic "Core" trait that must be implemented by a back-end
pub trait Core {
    // Create a new instance
    fn new() -> Self;

    // Reset the environment as if it was turned off and then back on
    fn reset(&mut self);

    // Run for the next "frame"
    fn run_next(&mut self);

    // Open ROM
    fn open_rom(&mut self, filename: &str);

    // Store the `video_refresh` callback
    //  - The back-end should invoke this when a "frame" of video is complete (generally during V-Blank of the guest machine)
    fn set_video_refresh(&mut self, cb: VideoRefresh);
}

// Generate extern methods to "expose" the core
#[macro_export]
macro_rules! ax_expose (($t:path) => {
  extern crate libc;
  use std::mem;
  use std::ffi::CStr;
  use axal::Core;
  use axal::VideoRefresh;

  #[no_mangle]
  pub unsafe extern "C" fn ax_new() -> *mut libc::c_void {
      // BUG(rust): Not sure how to use $t directly below
      use $t as t;

      mem::transmute(Box::new(t::new()))
  }

  #[no_mangle]
  pub unsafe extern "C" fn ax_delete(ptr: *mut libc::c_void) {
      let _drop_me: Box<$t> = mem::transmute(ptr);
  }

  #[no_mangle]
  pub unsafe extern "C" fn ax_reset(ptr: *mut libc::c_void) {
      (*(ptr as *mut $t)).reset();
  }

  #[no_mangle]
  pub unsafe extern "C" fn ax_set_video_refresh(ptr: *mut libc::c_void, cb: VideoRefresh) {
      (*(ptr as *mut $t)).set_video_refresh(cb);
  }

  #[no_mangle]
  pub unsafe extern "C" fn ax_open_rom(ptr: *mut libc::c_void, filename: *const libc::c_char) {
      let filename = CStr::from_ptr(filename).to_str().unwrap();

      (*(ptr as *mut $t)).open_rom(filename);
  }

  #[no_mangle]
  pub unsafe extern "C" fn ax_run_next(ptr: *mut libc::c_void) {
      (*(ptr as *mut $t)).run_next();
  }
});
