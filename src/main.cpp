#include <cstdio>
#include <iostream>

#include <QApplication>
#include <QSurfaceFormat>

#include "window.h"
#include "library.h"
#include "core.h"

static std::unique_ptr<ax::Window> _window;
static std::unique_ptr<ax::Core> _core;

void ax_video_refresh(uint8_t* buffer, uint32_t w, uint32_t h, uint32_t pitch) {
  if (_window && _core && _core->is_running()) {
    // TODO: Better name?
    _window->viewport().update_framebuffer(buffer, w, h);
  }
}

int main(int argc, char** argv) {
  // Load CHIP-8 core
  _core.reset(new ax::Core("./libaxal_chip_8.dylib"));

  // Insert predefined CHIP-8 ROM
  _core->insert_rom("/Users/mehcode/Documents/Games/Chip-8/Cave.ch8");

  // Set desired Open GL version: v3.3 CORE
  QSurfaceFormat fmt;
  fmt.setVersion(3, 3);
  fmt.setOption(QSurfaceFormat::DeprecatedFunctions, false);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setRenderableType(QSurfaceFormat::OpenGL);

  QSurfaceFormat::setDefaultFormat(fmt);

  // Create Qt Application
  QApplication app(argc, argv);

  // Create window
  _window.reset(new ax::Window());

  // Setup "end of video frame" handler
  _core->set_video_refresh(ax_video_refresh);

  // Show window
  _window->show();

  // Run application until "quit"
  int result = app.exec();

  // Destroy window (this must be destroyed _before_ the application)
  _window.reset(NULL);

  return result;
}
