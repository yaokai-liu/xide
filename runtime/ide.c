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
 * Filename: ide.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "ide.h"
#include "color.h"
#include "minmax.h"
#include "print.h"
#include "runtime-enum.h"
#include "runtime.h"
#include "texture-manage.h"
#include "utils.h"

GLFWwindow *ideInitGlfwGLContext(IDE *ide, int width, int height);

IDE *IDE_new(const char_t *workdir, const Allocator *allocator) {
  IDE *ide = allocator->calloc(1, sizeof(IDE));
  ide->allocator = allocator;
  ide->workdir = workdir;
  ide->atlasManager = Array_new(sizeof(TextureAtlas), enum_IDE_TEXTURE_ATLAS, allocator);
  ide->fontManager = FontManager_new(allocator);
  ide->drawTaskArray = Array_new(sizeof(DrawTask), enum_XGL_DRAW_TASK, allocator);
  ide->shaderProgramArray = Array_new(sizeof(GLuint), enum_XGL_SHADER_PROG, allocator);
  GLFWwindow *handle = ideInitGlfwGLContext(ide, 1000, 1000);
  ShaderInfo shaderInfos[][2] = {
    [DEFAULT_SHADER] = {{"shaders/vert-default.glsl", GL_VERTEX_SHADER},
     {"shaders/frag-default.glsl", GL_FRAGMENT_SHADER}},
    [DEFAULT_CHAR_SHADER] = {{"shaders/char-vert.glsl", GL_VERTEX_SHADER},
     {"shaders/char-frag.glsl", GL_FRAGMENT_SHADER}}
  };
  ide->defaultShader[DEFAULT_SHADER] = ideCompileShaders(ide, shaderInfos[DEFAULT_SHADER], 2);
  ide->defaultShader[DEFAULT_CHAR_SHADER] = ideCompileShaders(ide, shaderInfos[DEFAULT_CHAR_SHADER], 2);
  if (!ide->defaultShader[0] || !ide->defaultShader[1]) { IDE_destroy(ide); glfwTerminate(); }

  ide->mainWindow = Window_new(ide, handle, "xide");
  if (!ide->mainWindow) { return nullptr; }
  glfwSetWindowUserPointer(ide->mainWindow->handle, ide);

  return ide;
}
void IDE_destroy(IDE *ide) {
  Window_destroy(ide->mainWindow);
  Array_reset(ide->drawTaskArray, (destruct_t *) xglDestroyDrawTask);
  Array_destroy(ide->drawTaskArray);
  releasePrimeArray(ide->shaderProgramArray);
  Array_reset(ide->atlasManager, (destruct_t *) releaseTextureAtlas);
  Array_destroy(ide->atlasManager);
  FontManager_destroy(ide->fontManager);
  ide->allocator->free(ide);
}

GLFWwindow *ideInitGlfwGLContext(IDE *ide, int width, int height) {
  rt_message("Using GLFW Version: %d.%d, build from source code", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
  // Required OpenGL version: 4.6.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_WIN_DECO_NO_TITLE_BAR);

  // TODO: loadPluginsFrom(directory) async;
  // TODO: loadProjectFrom(directory) async;
  // TODO: setupUiFrom(filepath) main thread;

  GLFWwindow *handle = glfwCreateWindow(width, height, "", nullptr, nullptr);
  if (!handle) {
    const char_t *err_msg = nullptr;
    glfwGetError(&err_msg);
    rt_error("failed to create GLFW window: %s", err_msg);
    return nullptr;
  }
  // make context
  glfwMakeContextCurrent(handle);
  // set swap interval
  glfwSwapInterval(1);
  // initialize glad
  if (initializeGlad()) { return nullptr; }
  // set opengl viewport
  glViewport(0, 0, width, height);

  glfwSetWindowSizeCallback(handle, glfwWindowResize);
  glfwSetWindowRefreshCallback(handle, glfwWindowRefresh);
  return handle;
}
