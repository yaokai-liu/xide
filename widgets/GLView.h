/* License
 *
 * xide - An integrated development environment
 * Copyright (C) 2025 Yaokai Liu
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
 * Module Name: widgets
 * Filename: GLView.h
 * Creator: Yaokai Liu
 * Create Date: 2025-05-03
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_GLVIEW_H
#define XIDE_GLVIEW_H

#include "Widget.h"

typedef struct GLView {
  Widget SUPER;
} GLView;

void GLView_draw(Widget *_text);
void GLView_makeGraph(Widget *_text);
void *GLView_eventProcess(Widget *_text, uint32_t event_id, void *args);
void ideMakeGLView(IDE *ide, Widget *_text);


#endif  // XIDE_GLVIEW_H
