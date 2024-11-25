/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: runtime.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_RUNTIME_H
#define XIDE_RUNTIME_H

#include "draw.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "widgets.h"
#include "xgl-object.h"

#define rt_error(fmt, ...)   fprintf(stderr, "[ERROR] " fmt ".\n", __VA_ARGS__)
#define rt_message(fmt, ...) fprintf(stdout, "[INFO] " fmt ".\n", __VA_ARGS__)
#define rt_warning(fmt, ...) fprintf(stdout, "[WARNING] " fmt ".\n", __VA_ARGS__)
#define rt_debug(fmt, ...)   fprintf(stdout, "[DEBUG] " fmt ".\n", __VA_ARGS__)

void APIENTRY xglDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar *message, const void *userParam);

GLFWmonitor *switchMonitor(int index, int *width, int *height);

void ideSetWindowSize(GLFWwindow *handle, int width, int height);
void ideWindowRefreshCallback(GLFWwindow *handle);
void ideProcessInput(GLFWwindow *window);
IdeWindow *ideCreateWindow(GLFWwindow *handle, const Allocator *allocator);
void ideDestroyWindow(IdeWindow *window);

void ideDrawUI(IdeWindow *window);
void ideWindowAddTasks(IdeWindow *window, DrawTask *task, int count);

#endif  // XIDE_RUNTIME_H
