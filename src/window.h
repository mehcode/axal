#ifndef AXAL_WINDOW_H
#define AXAL_WINDOW_H 1

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMenu>

#include "viewport.h"

namespace ax {

class Window : public QMainWindow {
 public:
  Window();
  virtual ~Window() noexcept;

  // Return the active OpenGL viewport
  Viewport& viewport() noexcept;

  // Callbacks
  void set_open(void(*callback)(const char*)) {
    _open = callback;
  }

  void set_close(void(*callback)()) {
    _close = callback;
  }

 private:
  // Callbacks
  void (*_open)(const char*);
  void (*_close)();

  // Actions
  // NOTE: All this memory is _owned_ by Qt
  QAction* _action_open;
  QAction* _action_close;

};

}  // namespace ax

#endif  // AXAL_WINDOW_H
