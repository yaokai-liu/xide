/**
 * Project Name: xide
 * Module Name: components
 * Filename: window.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "window.h"

MainWindow *MainWindow_new(const Allocator *allocator) {
  MainWindow *window = allocator->calloc(1, sizeof(MainWindow));

  window->allocator = allocator;
  return window;
}