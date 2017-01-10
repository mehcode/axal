#ifndef AXAL_VIEWPORT_H
#define AXAL_VIEWPORT_H 1

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace ax {

class Player;

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  Viewport(QWidget* parent);
  virtual ~Viewport() noexcept;

  void update_framebuffer(void* buffer, uint32_t width, uint32_t height) {
    // if (!_texture) return;

    // Update (next) framebuffer
    _framebuffer = buffer;
    _width = width;
    _height = height;

    // Refresh
    update();
  }

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

 private:
  friend class Player;

  static void _video_refresh(void* userdata,
    uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch);

  GLuint _texture;

  void* _framebuffer;
  uint32_t _width;
  uint32_t _height;

};

}  // namespace ax

#endif  // AXAL_VIEWPORT_H
