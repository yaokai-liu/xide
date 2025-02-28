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
 * Module Name: runtime
 * Filename: xgl-object.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-7
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_XGL_OBJECT_H
#define XIDE_XGL_OBJECT_H

#include "array.h"
#include "glad/glad.h"

enum XGL_OBJECT : uint32_t {
  enum_XGL_NONE = 0x00,
  enum_XGL_VAO = 0x0001,
  enum_XGL_VBO = 0x0002,
  enum_XGL_EBO = 0x0003,

  enum_XGL_VERTEX = 0x0010,
  enum_XGL_COORD = 0x0020,
  enum_XGL_COLOR = 0x0030,
  enum_XGL_TEXTURE_COORD = 0x0040,
  enum_XGL_INDEX = 0x0050,
  enum_XGL_TEXTURE_UNIT = 0x0060,

  enum_XGL_UNIFORM = 0x0100,

  enum_XGL_POINT = 0x1000,
  enum_XGL_LINE = 0x2000,
  enum_XGL_AERA = 0x3000,

  enum_XGL_DRAW_TASK = 0x00010000,
  enum_XGL_SHADER_PROG = 0x00020000,
  enum_XGL_SHADER = 0x00030000,

  enum_ANY = 0xffffffff,
};

enum LOCATION_LAYER {
  LOC_COORD = 0,
  LOC_COLOR = 1,
  LOC_TEXTURE_COORD = 2,
  LOC_VIEWPORT = 3,
  LOC_TEXTURE_ATLAS_SIZE = 4,
  LOC_TEXTURE_UNIT = 5,
};

typedef GLuint iXGLVao;
typedef GLuint iXGLVbo;
typedef GLuint iXGLIbo;
typedef GLuint iXGLshProg;

typedef GLfloat XGLVector2D[2];
typedef GLfloat XGLVector3D[3];
typedef GLfloat XGLVector4D[4];
typedef GLfloat XGLCoord[4];
typedef GLfloat XGLColor[4];
typedef GLfloat XGLTexCoord[2];
typedef uint32_t XGLRgba;
typedef GLfloat Matrix[4][4];

typedef struct XGLVertex {
  XGLCoord coord;
  XGLColor color;
  XGLTexCoord tex_coord;
} XGLVertex;


enum UNIFORM_DATA_TYPE {
  UD_INT,
  UD_UINT,
  UD_FLOAT,
  UD_DOUBLE,
};

enum UNIFORM_SHAPE {
  US_1SCA,
  US_2SCA,
  US_3SCA,
  US_4SCA,
  US_1VEC,
  US_2VEC,
  US_3VEC,
  US_4VEC,
  US_2x2M,
  US_2x3M,
  US_2x4M,
  US_3x2M,
  US_3x3M,
  US_3x4M,
  US_4x2M,
  US_4x3M,
  US_4x4M,
};

#define uniform_type(u_shape, u_dtype) ((u_dtype) << 5 | ((u_shape) & 0x1f))
#define uniform_shape(u_type)          ((u_type) & 0x1f)
#define uniform_dtype(u_type)          ((u_type) >> 5)

typedef struct iXGLVUniform {
  int8_t u_type;
  int32_t u_locate;
} iXGLVUniform;

#endif  // XIDE_XGL_OBJECT_H
