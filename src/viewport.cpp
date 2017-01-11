#include "viewport.h"
#include <thread>

GLuint VBO;
GLuint EBO;

GLuint shader;

const char* vertex_shader_src = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main() {
  gl_Position = vec4(position, 1.0);
  TexCoord = texCoord;
}
)";

const char* fragment_shader_src = R"(
#version 330

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main() {
  color = texture(ourTexture, TexCoord);
  // color = vec4(0.3, 0.4, 0.4, 1.0);
}
)";

ax::Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), _texture(0), _vao(this) {
  setFocusPolicy(Qt::StrongFocus);

  // Setup cross-thread signal
  connect(this, &Viewport::framebufferUpdated, this, &Viewport::updateFramebuffer);

  // Configure key map
  _qkey_to_axkey[Qt::Key_A] = AXKEY_A;
  _qkey_to_axkey[Qt::Key_B] = AXKEY_B;
  _qkey_to_axkey[Qt::Key_C] = AXKEY_C;
  _qkey_to_axkey[Qt::Key_D] = AXKEY_D;
  _qkey_to_axkey[Qt::Key_E] = AXKEY_E;
  _qkey_to_axkey[Qt::Key_F] = AXKEY_F;
  _qkey_to_axkey[Qt::Key_G] = AXKEY_G;
  _qkey_to_axkey[Qt::Key_H] = AXKEY_H;
  _qkey_to_axkey[Qt::Key_I] = AXKEY_I;
  _qkey_to_axkey[Qt::Key_J] = AXKEY_J;
  _qkey_to_axkey[Qt::Key_K] = AXKEY_K;
  _qkey_to_axkey[Qt::Key_L] = AXKEY_L;
  _qkey_to_axkey[Qt::Key_M] = AXKEY_M;
  _qkey_to_axkey[Qt::Key_N] = AXKEY_N;
  _qkey_to_axkey[Qt::Key_O] = AXKEY_O;
  _qkey_to_axkey[Qt::Key_P] = AXKEY_P;
  _qkey_to_axkey[Qt::Key_Q] = AXKEY_Q;
  _qkey_to_axkey[Qt::Key_R] = AXKEY_R;
  _qkey_to_axkey[Qt::Key_S] = AXKEY_S;
  _qkey_to_axkey[Qt::Key_T] = AXKEY_T;
  _qkey_to_axkey[Qt::Key_U] = AXKEY_U;
  _qkey_to_axkey[Qt::Key_V] = AXKEY_V;
  _qkey_to_axkey[Qt::Key_W] = AXKEY_W;
  _qkey_to_axkey[Qt::Key_X] = AXKEY_X;
  _qkey_to_axkey[Qt::Key_Y] = AXKEY_Y;
  _qkey_to_axkey[Qt::Key_Z] = AXKEY_Z;

  _qkey_to_axkey[Qt::Key_1] = AXKEY_NUM1;
  _qkey_to_axkey[Qt::Key_2] = AXKEY_NUM2;
  _qkey_to_axkey[Qt::Key_3] = AXKEY_NUM3;
  _qkey_to_axkey[Qt::Key_4] = AXKEY_NUM4;
  _qkey_to_axkey[Qt::Key_5] = AXKEY_NUM5;
  _qkey_to_axkey[Qt::Key_6] = AXKEY_NUM6;
  _qkey_to_axkey[Qt::Key_7] = AXKEY_NUM7;
  _qkey_to_axkey[Qt::Key_8] = AXKEY_NUM8;
  _qkey_to_axkey[Qt::Key_9] = AXKEY_NUM9;
  _qkey_to_axkey[Qt::Key_0] = AXKEY_NUM0;

  _qkey_to_axkey[Qt::Key_Backspace] = AXKEY_BACKSPACE;
  _qkey_to_axkey[Qt::Key_Tab] = AXKEY_TAB;
  _qkey_to_axkey[Qt::Key_Return] = AXKEY_RETURN;
  _qkey_to_axkey[Qt::Key_Escape] = AXKEY_ESCAPE;
  _qkey_to_axkey[Qt::Key_Space] = AXKEY_SPACE;

  _qkey_to_axkey[Qt::Key_F1] = AXKEY_F1;
  _qkey_to_axkey[Qt::Key_F2] = AXKEY_F2;
  _qkey_to_axkey[Qt::Key_F3] = AXKEY_F3;
  _qkey_to_axkey[Qt::Key_F4] = AXKEY_F4;
  _qkey_to_axkey[Qt::Key_F5] = AXKEY_F5;
  _qkey_to_axkey[Qt::Key_F6] = AXKEY_F6;
  _qkey_to_axkey[Qt::Key_F7] = AXKEY_F7;
  _qkey_to_axkey[Qt::Key_F8] = AXKEY_F8;
  _qkey_to_axkey[Qt::Key_F9] = AXKEY_F9;
  _qkey_to_axkey[Qt::Key_F10] = AXKEY_F10;
  _qkey_to_axkey[Qt::Key_F11] = AXKEY_F11;
  _qkey_to_axkey[Qt::Key_F12] = AXKEY_F12;

  _qkey_to_axkey[Qt::Key_Print] = AXKEY_PRINTSCREEN;
  _qkey_to_axkey[Qt::Key_ScrollLock] = AXKEY_SCROLLLOCK;
  _qkey_to_axkey[Qt::Key_Pause] = AXKEY_PAUSE;

  _qkey_to_axkey[Qt::Key_Insert] = AXKEY_INSERT;
  _qkey_to_axkey[Qt::Key_Home] = AXKEY_HOME;
  _qkey_to_axkey[Qt::Key_PageUp] = AXKEY_PAGEUP;
  _qkey_to_axkey[Qt::Key_Delete] = AXKEY_DELETE;
  _qkey_to_axkey[Qt::Key_End] = AXKEY_END;
  _qkey_to_axkey[Qt::Key_PageDown] = AXKEY_PAGEDOWN;

  _qkey_to_axkey[Qt::Key_Right] = AXKEY_RIGHT;
  _qkey_to_axkey[Qt::Key_Left] = AXKEY_LEFT;
  _qkey_to_axkey[Qt::Key_Down] = AXKEY_DOWN;
  _qkey_to_axkey[Qt::Key_Up] = AXKEY_UP;
}

ax::Viewport::~Viewport() noexcept {
}

void ax::Viewport::initializeGL() {
  initializeOpenGLFunctions();

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenTextures(1, &_texture);
  _vao.create();

  // Compile: Vertex Shader
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader);

  GLint success;
  GLchar err[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, err);
    std::printf("error: vertex shader: compilation failed \n%s\n", err);
  }

  // Compile: Fragment Shader
  // TODO: Error handling
  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
  glCompileShader(fragment_shader);

  // Link: Shader
  shader = glCreateProgram();
  glAttachShader(shader, vertex_shader);
  glAttachShader(shader, fragment_shader);
  glLinkProgram(shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glClearColor(0.3f, 0.3f, 0.3f, 1);
  glLoadIdentity();

  _vao.bind();

  GLfloat vertices[] = {
    // Positions          // Texture Coords
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f,   // Top Right
     1.0f, -1.0f, 0.0f,   1.0f, 1.0f,   // Bottom Right
    -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,   // Bottom Left
    -1.0f,  1.0f, 0.0f,   0.0f, 0.0f    // Top Left
  };

  GLuint indices[] = {
    0, 1, 3,   // First Triangle
    1, 2, 3    // Second Triangle
  };

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  _vao.release();
}

void ax::Viewport::paintGL() {
  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader);

  glBindTexture(GL_TEXTURE_2D, _texture);

  _vao.bind();

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  _vao.release();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void ax::Viewport::resizeGL(int w, int h) {
  // [...]
}

void ax::Viewport::updateFramebuffer(void* framebuffer, unsigned w, unsigned h) {
  glBindTexture(GL_TEXTURE_2D, _texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  // Let Qt know we need to re-paint the viewport
  update();
}

void ax::Viewport::_video_refresh(void* userdata, uint8_t* data, uint32_t width, uint32_t height, uint32_t) {
  emit ((ax::Viewport*)userdata)->framebufferUpdated(data, (unsigned)width, (unsigned)height);
}

void ax::Viewport::keyPressEvent(QKeyEvent* event) {
  auto r = _qkey_to_axkey.find(event->key());
  if (r != _qkey_to_axkey.end()) {
    _key_state[r->second] = true;
  }
}

void ax::Viewport::keyReleaseEvent(QKeyEvent* event) {
  auto r = _qkey_to_axkey.find(event->key());
  if (r != _qkey_to_axkey.end()) {
    _key_state[r->second] = false;
  }
}
