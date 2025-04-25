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
#include "object-enum.h"
#include "print.h"
#include "runtime.h"
#include "texture-manage.h"
#include "utils.h"

#define DEFAULT_SHADER 0
#define DEFAULT_CHAR_SHADER 1

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

  ide->mainWindow = ideMakeWindow(ide, handle, "xide");
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

CharModelSet *ideGenCharModelSet(IDE *ide, const Font *font) {
  CharModelSet *set = FontManager_loadFont(ide->fontManager, font);
  if (!set) { return nullptr; }
  set = FontManager_realCharModelSet(ide->fontManager, set);
  if (set->atlas) { return set; }
  TextureAtlas atlas = {.unit = 0, .texture = 0, .width = 0, .height = 0};
  Array_append(ide->atlasManager, &atlas, 1);
  set->atlas = Array_length(ide->atlasManager);
  return set;
}

uint32_t ideUpdateCharacterTextureAtlas(IDE *ide, const Array /*<char_t>*/ *char_array, CharModelSet *set) {
  const Allocator *allocator = ide->allocator;
  TextureAtlas *atlas = Array_real_addr(ide->atlasManager, set->atlas - 1);
  const uint32_t old_width = atlas->width, old_height = atlas->height;
  uint32_t current_width = atlas->width + 1, current_height = atlas->height;
  Array *update_array = Array_new(sizeof(char_t), enum_IDE_CHAR, allocator);
  const char_t *character = Array_first_real(char_array);
  const char_t *last = Array_last_real(char_array);
  for (; character <= last; character++) {
    if (AVLTree_get(set->charTree, *character)) { continue; }
    FT_Error err = FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    if (err) {
      rt_error("Loading glyph of '%c' in %s failed", *character, set->font.path);
      continue;
    }
    const uint32_t sub_width = set->face->glyph->bitmap.width + 1;
    const uint32_t sub_height = set->face->glyph->bitmap.rows;
    const int32_t bearing_x = set->face->glyph->bitmap_left;
    const int32_t bearing_y = set->face->glyph->bitmap_top;
    const int64_t advance_x = set->face->glyph->advance.x;
    const int64_t advance_y = set->face->glyph->advance.y;
    const CharModel model = {
      .code = *character,
      .offset = current_width,
      .size = {[AXIS_X] = sub_width, [AXIS_Y] = sub_height},
      .bearing = {[AXIS_X] = bearing_x, [AXIS_Y] = bearing_y },
      .advance = {[AXIS_X] = advance_x, [AXIS_Y] = advance_y }
    };
    Array_append(set->modelArray, &model, 1);
    Array_append(update_array, character, 1);
    REFER(CharModel) v_model = Array_last_virt(set->modelArray);
    AVLTree_set(set->charTree, *character, v_model);
    current_height = max(current_height, sub_height + 1);
    current_width += sub_width + 2;
  }
  atlas->width = current_width;
  atlas->height = current_height;
  if (Array_length(update_array) == 0) {
    releasePrimeArray(update_array);
    return 0;
  }
  const uint32_t old_texture = atlas->texture;
  glCreateTextures(GL_TEXTURE_2D, 1, &atlas->texture);
  glTextureStorage2D(atlas->texture, 1, GL_R8, (GLsizei) current_width, (GLsizei) current_height);
  if (old_texture) {
    glCopyImageSubData(old_texture, GL_TEXTURE_2D, 0, 0, 0, 0, atlas->texture, GL_TEXTURE_2D, 0, 0, 0, 0,
                       (GLsizei) old_width - 1, (GLsizei) old_height, 0);
    glDeleteTextures(1, &old_texture);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  character = Array_first_real(update_array);
  last = Array_last_real(update_array);
  for (; character <= last; character++) {
    const CharModel *model = AVLTree_get(set->charTree, *character);
    model = Array_virt2real(set->modelArray, model);
    FT_Load_Char(set->face, *character, FT_LOAD_RENDER);
    const uint32_t offset = model->offset;
    const uint8_t *data = set->face->glyph->bitmap.buffer;
    glTextureSubImage2D(atlas->texture, 0, (GLint) offset, 0, (GLsizei) model->size[AXIS_X] - 1,
                        (GLsizei) model->size[AXIS_Y], GL_RED, GL_UNSIGNED_BYTE, data);
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(atlas->texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  uint32_t count = Array_length(update_array);
  releasePrimeArray(update_array);
  return count;
}

const CharModelSet *ideUpdateCharModelSet(IDE *ide, const Font *font, const Array /*<char_t>*/ *char_array) {
  CharModelSet *set = ideGenCharModelSet(ide, font);
  if (!set) { return nullptr; }
  ideUpdateCharacterTextureAtlas(ide, char_array, set);
  return set;
}

void ideMakeWidgetBox(IDE *ide, Widget *widget) {
  Vertex2D corners[] = {
    {(float) widget->box[BE_LEFT],  (float) widget->box[BE_TOP], RGB_CLEAR},
    {(float) widget->box[BE_RIGHT], (float) widget->box[BE_TOP], RGB_CLEAR},
    {(float) widget->box[BE_RIGHT], (float) widget->box[BE_BOTTOM], RGB_CLEAR},
    {(float) widget->box[BE_LEFT],  (float) widget->box[BE_BOTTOM], RGB_CLEAR},
  };
  Array *vertex_array = Array_new(sizeof(Vertex2D), enum_XGL_COORD, widget->allocator);
  Array_append(vertex_array, corners, 4);
  if (widget->drawTask) { xglDestroyDrawTask(widget->drawTask, ide->allocator); }
  widget->drawTask = ideCreatePolygon2D(vertex_array, 0, true, widget->allocator);
  GLuint *shader = (widget->shader)
                     ? Array_virt2real(ide->shaderProgramArray, widget->shader)
                     :Array_virt2real(ide->shaderProgramArray, ide->defaultShader[DEFAULT_SHADER]);
  xglBindShaderProgram(widget->drawTask, *shader);
  releasePrimeArray(vertex_array);
}

void ideMakeText(IDE *ide, Text *text) {
  PixelVertex2D pixel_anchor = {.coord = {0, 0}, .color = text->color};
  IdeWidget_local2global((Widget *) text, pixel_anchor.coord);
  Vertex2D anchor = {
    .coord = {[AXIS_X] = (float) pixel_anchor.coord[AXIS_X],
              [AXIS_Y] = (float) pixel_anchor.coord[AXIS_Y]},
    .color = pixel_anchor.color
  };
  float end[2] = {};
  if (text->SUPER.drawTask) { xglDestroyDrawTask(text->SUPER.drawTask, ide->allocator); }
  text->SUPER.drawTask = ideCreateTextStr2DByStr(ide, text->text, &anchor, 0.1f, text->mode,
                                         0, &text->font, end);
  if (text->SUPER.property & WP_BOX_AS_GEOMETRY) {
    text->SUPER.box[BE_RIGHT] = (int) end[AXIS_X] + 1;
    text->SUPER.box[BE_BOTTOM] = (int) end[AXIS_Y] + 1;
  } else {
    text->SUPER.box[BE_RIGHT] = text->SUPER.box[BE_LEFT] + (int) end[AXIS_X] + 1;
    text->SUPER.box[BE_BOTTOM] = text->SUPER.box[BE_TOP] + (int) end[AXIS_Y] + 1;
  }
  GLuint *shader = (text->SUPER.shader)
                 ? Array_virt2real(ide->shaderProgramArray, text->SUPER.shader)
                 :Array_virt2real(ide->shaderProgramArray, ide->defaultShader[DEFAULT_CHAR_SHADER]);
  xglBindShaderProgram(text->SUPER.drawTask, *shader);
}
