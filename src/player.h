#ifndef AXAL_WINDOW_H
#define AXAL_WINDOW_H 1

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMenu>

#include "viewport.h"
#include "runtime.h"

namespace ax {

// Core "Player" Window
class Player : public QMainWindow {
 public:
  Player(const char* filename);
  virtual ~Player() noexcept;

 private:
  // Return the active OpenGL viewport
  Viewport& viewport() noexcept;

  // Axal Runtime
  Runtime _runtime;

  // Actions
  // NOTE: All this memory is _owned_ by Qt
  QAction* _action_open;
  QAction* _action_close;
  QAction* _action_reset;

};

}  // namespace ax

#endif  // AXAL_WINDOW_H
