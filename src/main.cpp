#include <cstdio>
#include <iostream>
#include <thread>

#include <QApplication>
#include <QSurfaceFormat>
// #include <QMainWindow>
// #include <QWidget>
// #include <QMenu>
// #include <QMenuBar>
// #include <QMessageBox>

#include "window.h"
#include "library.h"

static bool _running = true;

void ax_main(axal::Library* lib, void* handle) {
  // Resolve `ax_run_next`
  auto fn = lib->get<void (void*)>("ax_run_next");

  // Invoke until told to quit
  while (_running) {
    fn(handle);
  }
}

void ax_on_video_frame(void* user_data, uint8_t* buffer, uint32_t w, uint32_t h, uint8_t stride) {
  axal::Window* window = (axal::Window*)user_data;
  window->viewport().update_framebuffer(buffer, w, h);
}

int main(int argc, char** argv) {
  axal::Library lib("./libaxal_chip_8.dylib");

  auto ax_new = lib.get<void*()>("ax_new");
  auto ax_delete = lib.get<void(void*)>("ax_delete");
  auto ax_reset = lib.get<void(void*)>("ax_reset");
  auto ax_set_on_video_frame = lib.get<void(void*, void (*)(void*, uint8_t*, uint32_t, uint32_t, uint8_t), void*)>("ax_set_on_video_frame");
  auto ax_open_rom = lib.get<void(void*, const char*)>("ax_open_rom");

  auto handle = ax_new();

  ax_open_rom(handle, "/Users/mehcode/Documents/Games/Chip-8/Cave.ch8");

  std::thread ax_thread(ax_main, &lib, handle);

  // Set desired Open GL version
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setOption(QSurfaceFormat::DeprecatedFunctions, false);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setRenderableType(QSurfaceFormat::OpenGL);

  QSurfaceFormat::setDefaultFormat(fmt);

  QApplication app(argc, argv);
  axal::Window window;

  // Setup "end of video frame" handler
  ax_set_on_video_frame(handle, ax_on_video_frame, &window);

  window.show();

  // Run application until "quit"
  int result = app.exec();

  // Disable and dispose of Core
  _running = false;
  ax_thread.join();
  ax_delete(handle);

  return result;
}
