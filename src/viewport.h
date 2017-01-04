#ifndef AXAL_VIEWPORT_H
#define AXAL_VIEWPORT_H 1

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace axal {

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  Viewport(QWidget* parent);
  virtual ~Viewport() noexcept;

  void update_framebuffer(void* buffer, uint32_t width, uint32_t height) {
    // if (!_texture) return;
    _framebuffer = buffer;
    _width = width;
    _height = height;
  }

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

 private:
  GLuint _texture;

  void* _framebuffer;
  uint32_t _width;
  uint32_t _height;

};

}  // namespace axal

#endif  // AXAL_VIEWPORT_H
