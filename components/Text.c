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
 * Module Name: components
 * Filename: Text.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-15
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "Text.h"
#include "draw.h"

void Text_draw(Widget *_text) {
  if (_text->drawTask) { ideDraw(_text->runtime, _text->drawTask); }
}
void Text_makeGraph(Widget *_text) {
  ideMakeText(_text->runtime, _text);
}

void ideMakeText(IDE *ide, Widget *_text) {
  Text *text = (Text *)_text;
  PixelVertex2D pixel_anchor = {.coord = {0, 0}, .color = text->color};

  IdeWidget_local2global((Widget *) text, pixel_anchor.coord);
  Vertex2D anchor = {
    .coord = {[AXIS_X] = (float) pixel_anchor.coord[AXIS_X],
              [AXIS_Y] = (float) pixel_anchor.coord[AXIS_Y]},
    .color = pixel_anchor.color
  };
  float fsize[2] = {};
  if (text->SUPER.drawTask) { xglDestroyDrawTask(text->SUPER.drawTask, ide->allocator); }
  text->SUPER.drawTask = ideCreateTextStr2DByStr(ide, text->text, &anchor, -0.1f, text->mode,
                                                 0, &text->font, fsize);
  uint32_t size[2] = { [AXIS_X] = (uint32_t) fsize[AXIS_X] + 1, [AXIS_Y] = (uint32_t) fsize[AXIS_Y] + 1 };
  if (text->SUPER.property & WP_BOX_AS_GEOMETRY) {
    text->SUPER.box[BE_R] = size[AXIS_X];
    text->SUPER.box[BE_B] = size[AXIS_Y];
  } else {
    text->SUPER.box[BE_R] = text->SUPER.box[BE_L] +  size[AXIS_X];
    text->SUPER.box[BE_B] = text->SUPER.box[BE_T] +  size[AXIS_Y];
  }
  GLuint *shader = (text->SUPER.shader)
                     ? Array_virt2real(ide->shaderProgramArray, text->SUPER.shader)
                     :Array_virt2real(ide->shaderProgramArray, ide->defaultShader[DEFAULT_CHAR_SHADER]);
  xglBindShaderProgram(text->SUPER.drawTask, *shader);
}
