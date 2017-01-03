#include "viewport.h"

axal::Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent) {
}

axal::Viewport::~Viewport() noexcept {
}

void axal::Viewport::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(1, 1, 1, 1);
  glLoadIdentity();
}

void axal::Viewport::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void axal::Viewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}
