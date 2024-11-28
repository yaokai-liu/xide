/**
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

enum LOCATION_LAYER {
  LOC_VERTEX = 0,
  LOC_COLOR = 1,
  LOC_TEXTURE = 2,
  LOC_WINDOW_SIZE = 3,
};

typedef GLuint iXGLVao;
typedef GLuint iXGLVbo;
typedef GLuint iXGLIbo;
typedef GLuint iXGLshProg;

typedef GLfloat XGLCoord[4];
typedef GLfloat XGLColor[4];
typedef GLfloat Matrix[4][4];

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

#define uniform_type(u_shape, u_dtype) ((u_dtype) << 5 | (u_shape & 0x1f))
#define uniform_shape(u_type)          ((u_type) & 0x1f)
#define uniform_dtype(u_type)          ((u_type) >> 5)

typedef struct iXGLVUniform {
  int8_t u_type;
  int32_t u_locate;
} iXGLVUniform;

#endif  // XIDE_XGL_OBJECT_H
