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
extern iXGLshProg BUILTIN_GRADUAL_SHADER_PROGRAM;
void ideSetWindowSize(GLFWwindow *handle, int width, int height) {
  glViewport(0, 0, width, height);
  IdeWindow *window = glfwGetWindowUserPointer(handle);
  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  window->viewport[0] = (float) viewport[0];
  window->viewport[1] = (float) viewport[1];
  window->viewport[2] = (float) viewport[2];
  window->viewport[3] = (float) viewport[3];
}

void ideWindowRefreshCallback(GLFWwindow *handle) {
  IdeWindow *window = glfwGetWindowUserPointer(handle);
  ideDrawUI(window);
  glFinish();
}

void ideProcessInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
}

void APIENTRY xglDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
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