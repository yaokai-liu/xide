/**
 * Project Name: xide
 * Module Name: computation-geometry
 * Filename: cg2d.c
 * Creator: Yaokai Liu
 * Create Date: 2024-11-21
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "cg2d.h"
#include "definition.h"
#include "runtime.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef max
  #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
  #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define square(x)         ((x) * (x))
#define square_diff(x, y) (square(x) - square(y))
#define vert_distance(x, y) \
  sqrtf(square((x)[AXIS_X] - (y)[AXIS_X]) + square((x)[AXIS_Y] - (y)[AXIS_Y]))

typedef float Point[2];

struct Triangle {
  float vertices[3][2];
  int indices[3];
  bool isBad;
  bool isExterior;
};
struct Circle {
  float center[2];
  float radius;
};

bool isSameEdge(const int (*edge1)[2], const int (*edge2)[2]) {
  bool b = ((*edge1)[0] == (*edge2)[0] && (*edge1)[1] == (*edge2)[1])
           || ((*edge1)[0] == (*edge2)[1] && (*edge1)[1] == (*edge2)[0]);
  return b;
}

bool edgeInTriangle(const int (*edge)[2], const struct Triangle *triangle) {
  int edges[3][2] = {
    {triangle->indices[0], triangle->indices[1]},
    {triangle->indices[1], triangle->indices[2]},
    {triangle->indices[2], triangle->indices[0]},
  };
  return isSameEdge(edge, &(edges[0])) || isSameEdge(edge, &(edges[1]))
         || isSameEdge(edge, &(edges[2]));
}

bool notBadTriangle(const struct Triangle *triangle) {
  return !triangle->isBad;
}

bool notExterior(const struct Triangle *triangle) {
  return !triangle->isExterior;
}

bool intersectSegment(const int l1[2], const int l2[2]) {
  const Point a = {};
}

void createSuperTriangle(const Array * const vertex_array, struct Triangle * const triangle) {
  const Point *vertices = Array_get(vertex_array, 0);
  const int count = (int) Array_length(vertex_array);
  float min_x = FLT_MAX;
  float max_x = FLT_MIN;
  for (int i = 0; i < count; i++) {
    min_x = min(min_x, vertices[i][AXIS_X]);
    max_x = max(max_x, vertices[i][AXIS_X]);
  }
  float min_y = FLT_MAX;
  float max_y = FLT_MIN;
  for (int i = 0; i < count; i++) {
    min_y = min(min_y, vertices[i][AXIS_Y]);
    max_y = max(max_y, vertices[i][AXIS_Y]);
  }
  const float center[2] = {
    [AXIS_X] = (float) (min_x + max_x) / 2.0f, [AXIS_Y] = (float) (min_y + max_y) / 2.0f};
  const float radius = sqrtf(square(max_x - min_x) + square(max_y - min_y));
  triangle->vertices[0][AXIS_X] = center[AXIS_X] + 0.5f * sqrtf(3) * radius;
  triangle->vertices[0][AXIS_Y] = center[AXIS_Y] - 0.5f * radius;
  triangle->indices[0] = count;
  triangle->vertices[1][AXIS_X] = center[AXIS_X];
  triangle->vertices[1][AXIS_Y] = center[AXIS_Y] + radius;
  triangle->indices[1] = count + 1;
  triangle->vertices[2][AXIS_X] = center[AXIS_X] - 0.5f * sqrtf(3) * radius;
  triangle->vertices[2][AXIS_Y] = center[AXIS_Y] - 0.5f * radius;
  triangle->indices[2] = count + 2;
  triangle->isExterior = true;
  triangle->isBad = false;
}

#define x(i) (triangle->vertices[(i) - 1][AXIS_X])
#define y(i) (triangle->vertices[(i) - 1][AXIS_Y])
void getCircumscribedCircle(const struct Triangle *triangle, struct Circle *circle) {
  const float A1 = 2 * (x(2) - x(1));
  const float A2 = 2 * (x(3) - x(2));
  const float B1 = 2 * (y(2) - y(1));
  const float B2 = 2 * (y(3) - y(2));
  const float C1 = square_diff(x(2), x(1)) + square_diff(y(2), y(1));
  const float C2 = square_diff(x(3), x(2)) + square_diff(y(3), y(2));
  const float delta = A1 * B2 - A2 * B1;
  if (fabsf(delta) < 1e-10) {
    circle->center[AXIS_X] = 0.0f;
    circle->center[AXIS_Y] = 0.0f;
    circle->radius = FLT_MAX;
  }
  circle->center[AXIS_X] = (C1 * B2 - C2 * B1) / delta;
  circle->center[AXIS_Y] = (A1 * C2 - A2 * C1) / delta;
  circle->radius = vert_distance(triangle->vertices[0], circle->center);
}
#undef x
#undef y

#define isRelatedSuperTriangle(triangle)                                  \
  (((triangle)->indices[0] >= count) || ((triangle)->indices[1] >= count) \
   || ((triangle)->indices[2] >= count))

Array *xglCreateDelaunayIndexArray(Array *vertex_array, const Allocator *allocator) {
  const int count = (int) Array_length(vertex_array);

  struct Triangle super_triangle = {};
  createSuperTriangle(vertex_array, &super_triangle);
  Array_append(vertex_array, &(super_triangle.vertices), 3);
  const Point *vertices = Array_get(vertex_array, 0);

  Array *triangles = Array_new(sizeof(struct Triangle), allocator);
  Array_append(triangles, &super_triangle, 1);
  for (int i = 0; i < count; i++) {
    const Point *vertex = Array_get(vertex_array, i);
    Array *bad_triangles = Array_new(sizeof(struct Triangle), allocator);
    for (int j = 0; j < Array_length(triangles); j++) {
      struct Triangle *triangle = Array_get(triangles, j);
      struct Circle circle = {};
      getCircumscribedCircle(triangle, &circle);
      if (vert_distance(*vertex, circle.center) <= circle.radius) {
        // the vertex is in the circle,
        // means the triangle is not a Delaunay triangle
        triangle->isBad = true;
        Array_append(bad_triangles, triangle, 1);
      }
    }
    Array *temp_array = Array_filter(triangles, (bool (*)(const void *)) notBadTriangle);
    Array_reset(triangles, nullptr);
    Array_destroy(triangles);
    triangles = temp_array;

    Array *polygon_edges = Array_new(sizeof(int[2]), allocator);
    for (int j = 0; j < Array_length(bad_triangles); j++) {
      struct Triangle *triangle = Array_get(bad_triangles, j);
      int edges[3][2] = {
        {triangle->indices[0], triangle->indices[1]},
        {triangle->indices[1], triangle->indices[2]},
        {triangle->indices[2], triangle->indices[0]},
      };
      for (int e = 0; e < 3; e++) {
        int(*edge)[2] = &edges[e];
        for (int k = 0; k < Array_length(bad_triangles); k++) {
          if (k == j) { continue; }
          struct Triangle *bad_triangle = Array_get(bad_triangles, k);
          if (edgeInTriangle(edge, bad_triangle)) { goto __edge_is_shared; }
        }
        Array_append(polygon_edges, edge, 1);
__edge_is_shared:;
      }
    }
    Array_reset(bad_triangles, nullptr);
    Array_destroy(bad_triangles);
    temp_array =
      Array_deduplicate(polygon_edges, (bool (*)(const void *, const void *)) isSameEdge);
    Array_reset(polygon_edges, nullptr);
    Array_destroy(polygon_edges);
    polygon_edges = temp_array;

    for (int j = 0; j < Array_length(polygon_edges); j++) {
      const int(*edge)[2] = Array_get(polygon_edges, j);
      Point edgeVertices[2] = {};
      edgeVertices[0][AXIS_X] = vertices[(*edge)[0]][AXIS_X];
      edgeVertices[0][AXIS_Y] = vertices[(*edge)[0]][AXIS_Y];
      edgeVertices[1][AXIS_X] = vertices[(*edge)[1]][AXIS_X];
      edgeVertices[1][AXIS_Y] = vertices[(*edge)[1]][AXIS_Y];
      struct Triangle triangle = {
        .vertices = {{edgeVertices[0][AXIS_X], edgeVertices[0][AXIS_Y]},
                     {edgeVertices[1][AXIS_X], edgeVertices[1][AXIS_Y]},
                     {(*vertex)[AXIS_X], (*vertex)[AXIS_Y]},},
        .indices = {(*edge)[0], (*edge)[1], i},
        .isBad = false,
      };
      triangle.isExterior = isRelatedSuperTriangle(&triangle);
      Array_append(triangles, &triangle, 1);
    }
    Array_reset(polygon_edges, nullptr);
    Array_destroy(polygon_edges);
  }
  Array *temp_array = Array_filter(triangles, (bool (*)(const void *)) notExterior);
  Array_reset(triangles, nullptr);
  Array_destroy(triangles);
  triangles = temp_array;

  Array *index_array = Array_new(sizeof(int), allocator);
  for (int i = 0; i < Array_length(triangles); i++) {
    const struct Triangle *triangle = Array_get(triangles, i);
    Array_append(index_array, &(triangle->indices), 3);
  }
  Array_reset(triangles, nullptr);
  Array_destroy(triangles);

  return index_array;
}