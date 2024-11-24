/**
 * Project Name: xide
 * Module Name: style
 * Filename: shader.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_SHADER_H
#define XIDE_SHADER_H

#include "allocator.h"
#include "glad/glad.h"

GLuint compileShader(char *path, GLenum type, const Allocator *allocator);

#endif  // XIDE_SHADER_H
