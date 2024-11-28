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
#include "widget.h"
#include <stdint.h>

struct WinMetaInfo {
  uint32_t posX;
  uint32_t posY;
  uint32_t width;
  uint32_t height;
  void *handle;
};

typedef struct Dialog {
  struct WinMetaInfo info;
  const Allocator *allocator;
} Dialog;

typedef struct MainWindow {
  struct WinMetaInfo info;
  const Allocator *allocator;
  Widget *topBar;
  Widget *rightBar;
  Widget *bottomBar;
  Widget *leftBar;
  Widget *central;
  Array *drawTaskList;  // Array<DrawTask>
  float viewport[4];
} IdeWindow;

#endif  // XIDE_WINDOW_H
