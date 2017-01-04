#include "window.h"
#include "viewport.h"

axal::Window::Window() {
  setWindowTitle("Axal");

  // Size
  auto width = 64 * 10;
  auto height = 32 * 10;
  resize(width, height);
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  // Layout
  setCentralWidget(new Viewport(this));
}

axal::Window::~Window() noexcept {
}
