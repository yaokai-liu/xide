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
 * Filename: Window.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_WINDOW_H
#define XIDE_WINDOW_H

#include "allocator.h"
#include "array.h"
#include "char_t.h"
#include <stdint.h>
#include "Text.h"

typedef struct Window {
  Widget SUPER;
  void * handle;
  Widget *central;
  Widget *bars[4];
} Window, Dialog;

void Window_draw(Widget *_window);
void Window_update(Widget *_window);

void Window_setTextTitle(Window *window, Text *text);
void Window_processInput(Window *window);
void Window_destroy(Window *window);


#endif  // XIDE_WINDOW_H
