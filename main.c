#include "runtime.h"
#include "shader.h"
#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  const Allocator * const allocator = &STDAllocator;

  if (!glfwInit()) { return -1; }
  rt_message("Using GLFW Version: %d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
  // Required OpenGL version: 4.6.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  int width = 1000, height = 800;
  GLFWmonitor *monitor = NULL;
  if (argc == -1) { monitor = switchMonitor(0, &width, &height); }
  GLFWwindow *handle = glfwCreateWindow(width, height, "glfwWindow title", monitor, NULL);
  if (!handle) {
    glfwTerminate();
    return -1;
  }
  glfwSwapInterval(1);
  glfwSetWindowSizeCallback(handle, ideSetWindowSize);
  glfwSetWindowRefreshCallback(handle, ideWindowRefreshCallback);
  glfwMakeContextCurrent(handle);

  int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  if (!status) { rt_error("failed to initialize GLAD: %d", 0); }
  rt_message("OpenGL Vendor: %s", glGetString(GL_VENDOR));
  rt_message("Using OpenGL Version: %s", glGetString(GL_VERSION));
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags) {
    rt_message("%s", "Debug Enabled");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(xglDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }

  // shader program
  GLuint vertexShader = compileShader("shader/vertex.glsl", GL_VERTEX_SHADER, allocator);
  GLuint fragmentShader = compileShader("shader/fragment.glsl", GL_FRAGMENT_SHADER, allocator);
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    rt_error("Failed to link shader program: \n%s", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  IdeWindow *mainWindow = ideCreateWindow(handle, allocator);
  DrawTask *task;
  //    Line lines[] = {
  //        {
  //            .start = {100, 100},
  //            .end = {700, 100},
  //            .colors = {0x00FF00FF, 0x00FF00FF},
  //        },
  //        {
  //            .start = {700, 100},
  //            .end = {700, 300},
  //            .colors = {0x00FF00FF, 0x00FF00FF},
  //        },
  //        {
  //            .start = {700, 500},
  //            .end = {100, 500},
  //            .colors = {0x00FF00FF, 0x00FF00FF},
  //        },
  //        {
  //            .start = {100, 500},
  //            .end = {100, 100},
  //            .colors = {0x00FF00FF, 0x00FF00FF},
  //        },
  //    };
  //    task = xglCreateLines(lines, 4, 0, mainWindow);
  //  xglBindShaderProgram(task, shaderProgram);
  //  ideWindowAddTasks(mainWindow, task, 1);
  //  allocator->free(task);

  Vertex vertices[] = {
    {.coord = {200, 400}, .color = 0xFFFFFFFF},
    {.coord = {300, 200}, .color = 0xFFFFFFFF},
    {.coord = {700, 100}, .color = 0xFFFFFFFF},
    {.coord = {800, 300}, .color = 0xFFFFFFFF},
    {.coord = {700, 600}, .color = 0xFFFFFFFF},
    {.coord = {900, 800}, .color = 0xFFFFFFFF},
    {.coord = {600, 700}, .color = 0xFFFFFFFF},
    {.coord = {400, 600}, .color = 0xFFFFFFFF},
  };

  task = xglCreatePolygon(vertices, 8, 0, true, allocator);
  xglBindShaderProgram(task, shaderProgram);
  ideWindowAddTasks(mainWindow, task, 1);
  allocator->free(task);

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  while (!glfwWindowShouldClose(handle)) {
    ideProcessInput(handle);
    ideDrawUI(mainWindow);
    glfwPollEvents();
  }

  ideDestroyWindow(mainWindow);
  glfwTerminate();
  return 0;
}
