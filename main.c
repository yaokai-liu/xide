#include "runtime.h"
#include "shader.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

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

  IdeWindow *mainWindow = ideCreateWindow(handle, allocator);

  // shader program
  GLuint vertexShader = compileShader("shader/vert-default.glsl", GL_VERTEX_SHADER, allocator);
  GLuint fragmentShader = compileShader("shader/frag-default.glsl", GL_FRAGMENT_SHADER, allocator);
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

  DrawTask *task;

  Vertex vertices[] = {
    {.coord = {200.0f, 400.0f}, .color = 0xFFFFFFFF},
    {.coord = {300.0f, 200.0f}, .color = 0xFF00FFFF},
    {.coord = {500.0f, 100.0f}, .color = 0xFFFF00FF},
    {.coord = {600.0f, 300.0f}, .color = 0x00FFFFFF},
    {.coord = {700.0f, 600.0f}, .color = 0xFF00FFFF},
    {.coord = {800.0f, 800.0f}, .color = 0xFFFF00FF},
    {.coord = {900.0f, 700.0f}, .color = 0x00FFFFFF},
    {.coord = {800.0f, 900.0f}, .color = 0xFFFF00FF},
    {.coord = {500.0f, 800.0f}, .color = 0xFFFF00FF},
    {.coord = {300.0f, 600.0f}, .color = 0xFFFF00FF},
  };
  Array *vertex_array = Array_new(sizeof(Vertex), allocator);
  Array_append(vertex_array, vertices, 10);
  task = xglCreatePolygon2D(vertex_array, 0, true, allocator);
  xglBindShaderProgram(task, shaderProgram);
  ideWindowAddTasks(mainWindow, task, 1);
  allocator->free(task);
  Array_reset(vertex_array, nullptr);

  for (int i = 0; i < 100; i ++) {
    Vertex vert = {
        .coord = {
          400 + 200 * cosf(2 * (float) M_PI / 100 * (float) i),
          400 + 200 * sinf(2 * (float) M_PI / 100 * (float) i)
        },
        .color = 0xFFFF00FF
    };
    Array_append(vertex_array, &vert, 1);
  }
  Vertex center = { .coord = {400.0f, 400.0f }, .color = 0xFFFF00FF};
  Array_append(vertex_array, &center, 1);
  task = xglCreateCurveArea2D(vertex_array, 0, true, true, allocator);
  xglBindShaderProgram(task, shaderProgram);
  ideWindowAddTasks(mainWindow, task, 1);
  allocator->free(task);
  releaseArray(vertex_array);

  Line lines[] = {
      {{100, 100, 0x00FF00FF},
       {700, 100, 0x00FF00FF},
      },
      {
        {700, 100, 0x00FF00FF},
        {700, 500, 0x00FF00FF},
      },
      {
        {700, 500, 0x00FF00FF},
        {100, 500, 0x00FF00FF},
      },
      {
        {100, 500, 0x00FF00FF},
        {100, 100, 0x00FF00FF},
      },
  };
  Array *line_array = Array_new(sizeof(Line), allocator);
  Array_append(line_array, lines, 4);
  task = xglCreatePixelLines(line_array, 0, allocator);
  xglBindShaderProgram(task, shaderProgram);
  ideWindowAddTasks(mainWindow, task, 1);
  allocator->free(task);
  releaseArray(line_array);

  glLineWidth(2);
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
