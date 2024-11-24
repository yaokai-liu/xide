/**
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

GLuint compileShader(char *path, GLenum type, const Allocator *allocator) {
  FILE *file = NULL;
  if (fopen_s(&file, path, "r") != 0) {
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
