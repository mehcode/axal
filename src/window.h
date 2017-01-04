#ifndef AXAL_WINDOW_H
#define AXAL_WINDOW_H 1

#include <QMainWindow>
#include "viewport.h"

namespace axal {

class Window : public QMainWindow {
 public:
  Window();
  virtual ~Window() noexcept;

  axal::Viewport& viewport() {
    return *((axal::Viewport*)centralWidget());
  }

};

}  // namespace axal

#endif  // AXAL_WINDOW_H
