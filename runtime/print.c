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
 * Filename: print.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-22
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "print.h"
#include "draw.h"
#include "ide.h"
#include "runtime.h"

void xglPrintText(const DrawTask *task, const GLfloat viewport[4]) {
  glUseProgram(task->program);
  glBindVertexArray(task->VAO);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindTextureUnit(task->texture_unit, task->texture);
  int loc_viewport = glGetUniformLocation(task->program, "viewport");
  int loc_tex = glGetUniformLocation(task->program, "tex");
  glProgramUniform4fv(task->program, loc_viewport, 1, viewport);
  glProgramUniform1i(task->program, loc_tex, (GLint) task->texture_unit);
  glDrawElements(GL_TRIANGLES, task->n_index, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
