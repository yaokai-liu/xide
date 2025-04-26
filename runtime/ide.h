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
 * Module Name: runtime
 * Filename: ide.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_IDE_H
#define XIDE_IDE_H

#include "Window.h"
#include "array.h"
#include "font-manage.h"
#include "stack.h"

typedef struct IDE {
  const Allocator *allocator;
  const char_t *workdir;
  Window *mainWindow;
  Array *drawTaskArray;  // Array<DrawTask>
  Array *shaderProgramArray;  // Array<GLuint>
  FontManager *fontManager;
  TextureAtlasManager *atlasManager;
  Widget *hoveredWidget;

  REFER(GLuint) defaultShader[2];
} IDE;

IDE *IDE_new(const char_t *workdir, const Allocator *allocator);
void IDE_destroy(IDE *ide);

#endif  // XIDE_IDE_H
