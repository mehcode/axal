#include "viewport.h"

GLuint VBO;
GLuint VAO;
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

ax::Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), _texture(0), _framebuffer(nullptr) {
}

ax::Viewport::~Viewport() noexcept {
}

void ax::Viewport::initializeGL() {
  initializeOpenGLFunctions();

  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenTextures(1, &_texture);

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

  glClearColor(0.2f, 0.3f, 0.3f, 1);
  glLoadIdentity();

  glBindVertexArray(VAO);

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

  glBindVertexArray(0);
}

void ax::Viewport::paintGL() {
  glViewport(0, 0, width(), height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader);

  glBindTexture(GL_TEXTURE_2D, _texture);
  glBindVertexArray(VAO);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

void ax::Viewport::resizeGL(int w, int h) {
  // [...]
}

void ax::Viewport::_video_refresh(void* userdata, uint8_t* data, uint32_t width, uint32_t height, uint32_t) {
  ax::Viewport* viewport = (ax::Viewport*)userdata;

  viewport->glBindTexture(GL_TEXTURE_2D, viewport->_texture);

  // TODO: Pretty sure these should be done once in init
  viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  viewport->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  viewport->glGenerateMipmap(GL_TEXTURE_2D);

  viewport->glBindTexture(GL_TEXTURE_2D, 0);
}
