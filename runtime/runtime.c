/**
 * Project Name: xide
 * Module Name: runtime
 * Filename: runtime.c
 * Creator: Yaokai Liu
 * Create Date: 2024-11-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "runtime.h"

extern DrawTask *TASK;
extern GLfloat viewSize[2];
extern iXGLshProg BUILTIN_GRADUAL_SHADER_PROGRAM;

void drawUI(GLFWwindow *window) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  //    xglDrawLines(TASK, viewSize);
  xglDrawSolidArea(TASK, viewSize);
  //    xglDrawPolyline(TASK, viewSize);
  glfwSwapBuffers(window);
}