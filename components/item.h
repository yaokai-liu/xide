/**
 * Project Name: xide
 * Module Name: components
 * Filename: item.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-9
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_ITEM_H
#define XIDE_ITEM_H

#include <stdint.h>

typedef struct Item {
  uint16_t config[4];
  void *content;
} Item;

#endif  // XIDE_ITEM_H
