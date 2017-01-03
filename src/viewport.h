#ifndef AXAL_VIEWPORT_H
#define AXAL_VIEWPORT_H 1

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace axal {

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  Viewport(QWidget* parent);
  virtual ~Viewport() noexcept;

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

};

}  // namespace axal

#endif  // AXAL_VIEWPORT_H
