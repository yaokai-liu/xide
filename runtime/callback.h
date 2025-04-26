/* License
 *
 * ${PROJ_DESCRIPTION}
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
 * Filename: callback.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-26
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_CALLBACK_H
#define XIDE_CALLBACK_H

void ideCallback_windowRefresh(GLFWwindow *handle);
void ideCallback_windowResize(GLFWwindow *handle, int width, int height);
void ideCallback_cursorPosition(GLFWwindow* handle, double pos_x, double pos_y);
void ideCallback_cursorEnterOrLEave(GLFWwindow* handle, int entered);

void APIENTRY xglCallback_debugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar *message, const void *userParam);


#endif  // XIDE_CALLBACK_H
