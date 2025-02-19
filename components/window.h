/**
 * Project Name: xide
 * Module Name: components
 * Filename: window.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_WINDOW_H
#define XIDE_WINDOW_H

#include "allocator.h"
#include "array.h"
#include "char_t.h"
#include "widget.h"
#include <stdint.h>

struct WinMetaInfo {
  void *handle;
  int geometry[4];
  float viewport[4];
  const char_t *title;
};

typedef struct Dialog {
  struct WinMetaInfo info;
  const Allocator *allocator;
} Dialog;

typedef struct MainWindow {
  struct WinMetaInfo info;
  const Allocator *allocator;
  const char_t *workdir;
  Array *drawTaskArray;  // Array<DrawTask>
  Array *shaderArray;  // Array<GLuint>
  Array *shaderProgramArray;  // Array<GLuint>
  Widget *topBar;
  Widget *rightBar;
  Widget *bottomBar;
  Widget *leftBar;
  Widget *central;
} IdeWindow;

#endif  // XIDE_WINDOW_H
