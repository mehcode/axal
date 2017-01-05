#ifndef AXAL_WINDOW_H
#define AXAL_WINDOW_H 1

#include <QMainWindow>
#include "viewport.h"

namespace ax {

class Window : public QMainWindow {
 public:
  Window();
  virtual ~Window() noexcept;

  // Return the active OpenGL viewport
  Viewport& viewport() noexcept;

};

}  // namespace ax

#endif  // AXAL_WINDOW_H
