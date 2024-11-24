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

void drawUI(GLFWwindow *window);

void setWindowSize(GLFWwindow *window, int width, int height);
void windowRefreshCallback(GLFWwindow *window);
void processInput(GLFWwindow *window);
MainWindow *setupWindow(GLFWwindow *handle, const Allocator *allocator);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam);
#endif  // XIDE_RUNTIME_H
