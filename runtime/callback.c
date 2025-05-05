/**
 * xide - An integrated development environment
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
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

void ideCallback_windowResize(GLFWwindow *handle, int width, int height) {
  glViewport(0, 0, width, height);
  Window *window = glfwGetWindowUserPointer(handle);
  GLint viewport[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewport);
  ideResizeWindow(window->SUPER.runtime, viewport);
}

void ideCallback_windowRefresh(GLFWwindow *handle) {
  Window *window = glfwGetWindowUserPointer(handle);
  ideDrawUiOnce(window->SUPER.runtime);
  glFinish();
}
void ideCallback_cursorPosition(GLFWwindow* handle, double pos_x, double pos_y) {
  Widget *_window = glfwGetWindowUserPointer(handle);
  uint32_t pos_hovered[2] = { [AXIS_X] = (int) pos_x, [AXIS_Y] = (int) pos_y };
  ideUpdateHoveredWidget(_window->runtime, pos_hovered);
  uint32_t pos_moved[2] = { [AXIS_X] = (int) pos_x, [AXIS_Y] = (int) pos_y };
  ideUpdateMouseMovement(_window->runtime, pos_moved);
}

void ideCallback_cursorEnterOrLEave(GLFWwindow* handle, int entered) {
  Widget *_window = glfwGetWindowUserPointer(handle);
  if (entered) {
    _window->runtime->hoveredWidget = _window;
  } else {
    uint32_t cursor_position[2] = { [AXIS_X] = -1, [AXIS_Y] = -1 };
    ideUpdateHoveredWidget(_window->runtime, cursor_position);
    _window->runtime->hoveredWidget = nullptr;
  }
}

void ideCallback_mouseButtonEvent(GLFWwindow* handle, int button, int action, int mods) {
  Widget *_window = glfwGetWindowUserPointer(handle);
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    uint32_t event = enum_EVENT_NONE;
    switch (action) {
      case GLFW_PRESS: { event = enum_EVENT_MOUSE_PRESS; break; }
      case GLFW_RELEASE: { event = enum_EVENT_MOUSE_RELEASE; break; }
      default:{}
    }
    idePassMouseLeftButtonEvent(_window->runtime, event, mods);
  }
}

void ideCallback_keyboardKeyEvent(Window *window) {
  GLFWwindow *handle = window->handle;
  if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(handle, true); }
}

void APIENTRY xglCallback_debugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
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