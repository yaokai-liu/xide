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
 * Filename: runtime-msg.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-19
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_RUNTIME_MSG_H
#define XIDE_RUNTIME_MSG_H

#include <stdio.h>

#define rt_error(fmt, ...)   fprintf(stderr, "[ERROR] " fmt ".\n", ##__VA_ARGS__)
#define rt_message(fmt, ...) fprintf(stdout, "[INFO] " fmt ".\n", ##__VA_ARGS__)
#define rt_warning(fmt, ...) fprintf(stdout, "[WARNING] " fmt ".\n", ##__VA_ARGS__)
#define rt_debug(fmt, ...)   fprintf(stdout, "[DEBUG] " fmt ".\n", ##__VA_ARGS__)


#endif  // XIDE_RUNTIME_MSG_H
