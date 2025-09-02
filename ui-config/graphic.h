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
 * Module Name: ui-config
 * Filename: graphic.h
 * Creator: Yaokai Liu
 * Create Date: 2025-09-01
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_UI_CONFIG_GRAPHIC_H
#define XIDE_UI_CONFIG_GRAPHIC_H

#include <stdint.h>
#include "inherit.h"
#include "ide-types.h"
#include "allocator.h"
#include "array.h"

typedef struct Graphic Graphic;

typedef void fn_make(Graphic *graphic);
typedef bool fn_area(Graphic *graphic, uint32_t local_coord[2]);


typedef struct Graphic {
  /**
   * @description the widget the graphic bound on
   */
  Widget *widget;
  /**
   * @description The graphic making program, making the graphic to a draw task
   * @tags OVERRIDE PASSDOWN
   * @param widget the widget itself
   */
  fn_make * OVERRIDE PASSDOWN funcMake;
  /**
   * @description
   * Determine if a position in the widget box is in the widget range.
   * To determine if it responses the cursor events in widget's box.
   * @tags OVERRIDE NONEPASS
   * @param widget the widget itself
   * @param local_coord the given position,
   * has been converted to widget local coord
   * @default nullptr, means response area of the widget is whole widget box
   */
  fn_area * OVERRIDE NONEPASS funcRange;
  /**
   * @description   the graphic shader of the widget
   * @default       ide's default shader or ide's default text shader
   * @type          REFER(uint32_t)
   *                because in OpenGL, the shader's handle is an `uint32_t`
   */
  REFER(uint32_t) shader;
  /**
   * @description   data used when the graphic running
   * @type          Array<uint8_t>
   */
   Array *data;
} Graphic;

#endif  // XIDE_UI_CONFIG_GRAPHIC_H
