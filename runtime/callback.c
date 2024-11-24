/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: callback.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "runtime.h"
#include "widgets.h"
#include <stdbool.h>
#include <stdio.h>
extern DrawTask *TASK;
extern GLfloat viewSize[2];
extern iXGLshProg BUILTIN_GRADUAL_SHADER_PROGRAM;
void setWindowSize(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  viewSize[0] = (float) width;
  viewSize[1] = (float) height;
}

void windowRefreshCallback(GLFWwindow *window) {
  drawUI(window);
  glFinish();
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
}

MainWindow *setupWindow(GLFWwindow *handle, const Allocator *allocator) {
  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;
  MainWindow *mainWindow = MainWindow_new(allocator);
  mainWindow->info.handle = handle;
  int pos_x, pos_y, width, height;
  glfwGetWindowPos(handle, &pos_x, &pos_y);
  glfwGetWindowSize(handle, &width, &height);
  mainWindow->info.posX = pos_x;
  mainWindow->info.posY = pos_y;
  mainWindow->info.width = width;
  mainWindow->info.height = height;
  return mainWindow;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) { return; }
  rt_debug("Debug message (%d): %s", id, message);

  switch (source) {
    case GL_DEBUG_SOURCE_API: rt_debug("%s", "Source: API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: rt_debug("%s", "Source: Window System"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: rt_debug("%s", "Source: Shader Compiler"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: rt_debug("%s", "Source: Third Party"); break;
    case GL_DEBUG_SOURCE_APPLICATION: rt_debug("%s", "Source: Application"); break;
    case GL_DEBUG_SOURCE_OTHER: rt_debug("%s", "Source: Other"); break;
    default: {
    }
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR: rt_debug("%s", "Type: Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: rt_debug("%s", "Type: Deprecated Behaviour"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: rt_debug("%s", "Type: Undefined Behaviour"); break;
    case GL_DEBUG_TYPE_PORTABILITY: rt_debug("%s", "Type: Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: rt_debug("%s", "Type: Performance"); break;
    case GL_DEBUG_TYPE_MARKER: rt_debug("%s", "Type: Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: rt_debug("%s", "Type: Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP: rt_debug("%s", "Type: Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER: rt_debug("%s", "Type: Other"); break;
    default: {
    }
  }
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: rt_debug("%s", "Severity: high"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: rt_debug("%s", "Severity: medium"); break;
    case GL_DEBUG_SEVERITY_LOW: rt_debug("%s", "Severity: low"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: rt_debug("%s", "Severity: notification"); break;
    default: {
    }
  }
}