#include <cstdio>
#include <iostream>

#include <QApplication>
#include <QSurfaceFormat>

#include "player.h"

extern "C" int axal_main(int argc, char** argv) {
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
  // TODO: Have proper argument parser
  ax::Player player(argc == 0 ? NULL : argv[1]);

  // Show
  player.show();

  // Run application until "quit"
  return app.exec();
}

// NOTE: Set by cmake when building as an executable
#ifdef AXAL_MAIN

int main(int argc, char** argv) {
  return axal_main(argc, argv);
}

#endif
