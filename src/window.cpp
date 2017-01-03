#include "window.h"
#include "viewport.h"

axal::Window::Window() {
  setWindowTitle("Axal");

  // Size
  auto width = 160 * 4;
  auto height = 140 * 4;
  resize(width, height);
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  // Layout
  setContentsMargins(1, 1, 1, 1);
  setCentralWidget(new Viewport(this));
}

axal::Window::~Window() noexcept {
}
