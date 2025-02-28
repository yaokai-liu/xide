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
  Widget *topBar;
  Widget *rightBar;
  Widget *bottomBar;
  Widget *leftBar;
  Widget *central;
} IdeWindow;

#endif  // XIDE_WINDOW_H
