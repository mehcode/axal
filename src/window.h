#ifndef AXAL_WINDOW_H
#define AXAL_WINDOW_H 1

#include <QMainWindow>

namespace axal {

class Window : public QMainWindow {
 public:
  Window();
  virtual ~Window() noexcept;

};

}  // namespace axal

#endif  // AXAL_WINDOW_H
