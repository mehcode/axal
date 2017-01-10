#include <cstdio>
#include <iostream>

#include <QApplication>
#include <QSurfaceFormat>

#include "player.h"

static std::unique_ptr<ax::Player> _player;
// static ax::Runtime _runtime;

// void ax_video_refresh(uint8_t* buffer, uint32_t w, uint32_t h, uint32_t pitch) {
//   if (_window && _runtime.is_loaded() && _runtime.is_running()) {
//     _window->viewport().update_framebuffer(buffer, w, h);
//   }
// }

// void ax_open(const char* filename) {
//   if (_runtime.is_loaded()) {
//     _runtime.pause();
//     _runtime.rom_insert(filename);
//     _runtime.reset();
//     _runtime.resume();
//   }
// }

// void ax_close() {
//   if (_runtime.is_loaded()) {
//     _runtime.pause();
//     _runtime.rom_remove();
//   }
// }

// void ax_reset() {
//   if (_runtime.is_loaded()) {
//     _runtime.pause();
//     _runtime.reset();
//     _runtime.resume();
//   }
// }

int main(int argc, char** argv) {
  // Set desired Open GL version: v3.3 CORE
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setOption(QSurfaceFormat::DeprecatedFunctions, false);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setRenderableType(QSurfaceFormat::OpenGL);

  QSurfaceFormat::setDefaultFormat(fmt);

  // Create Qt Application
  QApplication app(argc, argv);

  // Create player
  ax::Player player(argv[1]);

  // Setup "end of video frame" handler
  // _runtime.set_video_refresh(ax_video_refresh);

  // Connect actions/commands
  // _window->connect_open(ax_open);
  // _window->connect_close(ax_close);
  // _window->connect_reset(ax_reset);

  // Show
  player.show();

  // Run application until "quit"
  return app.exec();
}
