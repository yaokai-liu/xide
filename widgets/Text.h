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
 * Filename: Text.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_TEXT_H
#define XIDE_TEXT_H

#include "Widget.h"
#include "font-manage.h"

typedef struct Text {
  Widget SUPER;
  string_t text;
  Font    font;
  uint32_t mode;
  uint32_t color;
} Text;

void Text_draw(Widget *_text);
void Text_makeGraph(Widget *_text);
void *Text_eventProcess(Widget *_text, uint32_t event_id, void *args);
void ideMakeText(IDE *ide, Widget *_text);

#endif  // XIDE_TEXT_H
