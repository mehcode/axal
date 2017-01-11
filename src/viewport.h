#ifndef AXAL_VIEWPORT_H
#define AXAL_VIEWPORT_H 1

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QKeyEvent>
#include <unordered_map>

#include "axal.h"

namespace ax {

class Player;
class Runtime;

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions {
 Q_OBJECT

 public:
  Viewport(QWidget* parent);
  virtual ~Viewport() noexcept;

 public slots:
  void updateFramebuffer(void* framebuffer, unsigned w, unsigned h);

 signals:
  void framebufferUpdated(void* framebuffer, unsigned w, unsigned h);

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

  virtual void keyPressEvent(QKeyEvent*);
  virtual void keyReleaseEvent(QKeyEvent*);

 private:
  friend class Player;
  friend class Runtime;

  static void _video_refresh(void* userdata,
    uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch);

  GLuint _texture;
  QOpenGLVertexArrayObject _vao;

  std::unordered_map<int, axal_key> _qkey_to_axkey;
  std::unordered_map<axal_key, bool> _key_state;

};

}  // namespace ax

#endif  // AXAL_VIEWPORT_H
