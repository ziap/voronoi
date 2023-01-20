#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "voronoi.h"

#define WIDTH 960
#define HEIGHT 720

float frand() { return (float)rand() / (float)RAND_MAX; }
float fsin(float x) { return sin(x); }
float fcos(float x) { return cos(x); }

void resize_callback(GLFWwindow *window, int new_w, int new_h) {
  (void)window;
  resize(new_w, new_h);
}

void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;
  mouse_move(xpos, ypos);
}

int glCreateBuffer() {
  unsigned int buf;
  glCreateBuffers(1, &buf);
  return buf;
}

void glSetShaderSource(unsigned int shader, const char *src) {
  glShaderSource(shader, 1, &src, NULL);
}

int glGetShaderParameter(unsigned int shader, unsigned int pname) {
  int param;
  glGetShaderiv(shader, pname, &param);
  return param;
}

void message_callback(
  GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
  const GLchar *message, const void *userParam
) {
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  fprintf(
    stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x\n%s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
    message
  );
}

int main(void) {
  if (!glfwInit()) return -1;

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Voronoi", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    return -1;
  }

  glfwSwapInterval(0);

  printf("Created window with OpenGL version: %s\n", glGetString(GL_VERSION));

  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetCursorPosCallback(window, cursor_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);

  double last = glfwGetTime();
  double last_display = last;
  double dt = 0;

  unsigned frames = 0;

  resize(WIDTH, HEIGHT);
  render_init();

  srand(last);
  while (!glfwWindowShouldClose(window)) {
    render_update(dt);
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames += 1;
    double now = glfwGetTime();
    if (now - last_display >= 1) {
      char title[64];
      sprintf(title, "Voronoi - FPS: %d", frames);
      glfwSetWindowTitle(window, title);
      frames = 0;
      last_display = now;
    }
    dt = now - last;
    last = now;
  }

  glfwTerminate();
  return 0;
}
