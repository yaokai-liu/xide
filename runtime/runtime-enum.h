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
 * Filename: runtime-enum.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-24
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_RUNTIME_ENUM_H
#define XIDE_RUNTIME_ENUM_H

enum IDE_OBJECT {
  enum_IDE_NONE = 0,
  enum_IDE_CHAR,
  enum_IDE_FONT,
  enum_IDE_CHAR_MODEL,
  enum_IDE_CHAR_MODEL_SET,
  enum_IDE_TEXTURE_ATLAS,
};

enum IDE_EVENT {
  enum_EVENT_NONE,
  enum_EVENT_CURSOR_ENTER,
  enum_EVENT_CURSOR_MOVE,
  enum_EVENT_CURSOR_LEAVE,
  enum_EVENT_MOUSE_PRESS,
  enum_EVENT_MOUSE_RELEASE,
  enum_EVENT_RE_GEOMETRY,
  enum_EVENT_MAKE_GRAPHIC,
};

#define DEFAULT_SHADER 0
#define DEFAULT_CHAR_SHADER 1


#endif  // XIDE_RUNTIME_ENUM_H
