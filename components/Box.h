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
 * Module Name: components
 * Filename: Box.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_BOX_H
#define XIDE_BOX_H

#include "Widget.h"

#define BOX_DEFAULT_STOKE 20

typedef struct Box {
  Widget SUPER;
  Array *children; // Array<Widget *>
  Array *corners; // Array<PixelVertex2D>
} Box;

void Box_update(Widget *_box);
void Box_draw(Widget *_box);
Widget *Box_getSubWidget(Widget *_box, uint32_t coord[2]);
void *Box_eventProcess(Widget *_box, uint32_t event_id, void *args);
void ideMakeBox(IDE *ide, Widget *_box);

void Box_append(Box *box, Widget *child);

#endif  // XIDE_BOX_H
