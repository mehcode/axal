#include "window.h"
#include "viewport.h"

ax::Window::Window() {
  setContentsMargins(1, 1, 1, 1);
  setWindowTitle("Axal");

  // Size
  auto width = (64 * 10) + 1;
  auto height = (32 * 10) + 1;
  resize(width, height);
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);

  // Layout
  setCentralWidget(new Viewport(this));
}

ax::Window::~Window() noexcept {
}

auto ax::Window::viewport() noexcept -> Viewport& {
  return *((Viewport*)centralWidget());
}
