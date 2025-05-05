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
 * Module Name:
 * Filename: main.c
 * Creator: Yaokai Liu
 * Create Date: 2025-2-22
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "ide.h"
#include "print.h"
#include "runtime-enum.h"
#include "runtime.h"
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
  #include <direct.h>
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
  const Allocator * const allocator = &STDAllocator;

  char_t workdir[PATH_MAX] = {};
  getcwd(workdir, PATH_MAX);

  if (!glfwInit()) { return -1; }
  IDE *ide = IDE_new(workdir, 200, 200, allocator);
  if (!ide) { glfwTerminate(); return -1; }

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ideShowWindow(ide);

  IDE_destroy(ide);
  glfwTerminate();
  return 0;
}
