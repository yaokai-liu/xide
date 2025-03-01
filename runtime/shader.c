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
 * Module Name: style
 * Filename: shader.c
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "shader.h"
#include "runtime.h"
#include <stdio.h>

GLuint compileShader(const char_t *path, const GLenum type, const Allocator *allocator) {
  FILE *file = fopen(path, "r");
  if (!file) {
    rt_error("Failed to open shader file: '%s'", path);
    return 0;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  GLchar *source = allocator->malloc((sizeof(char) * length) + 1);
  fread((void *) source, sizeof(char), length, file);
  source[length] = 0;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar **) &source, NULL);
  glCompileShader(shader);
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    rt_error("Failed to compile shader '%s': \n%s\n", path, infoLog);
  }
  return shader;
}
