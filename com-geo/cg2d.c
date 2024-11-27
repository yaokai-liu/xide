/**
 * Project Name: xide
 * Module Name: com-geo
 * Filename: cg2d.c
 * Creator: Yaokai Liu
 * Create Date: 2024-11-21
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "cg2d.h"
#include "definition.h"
#include <float.h>
#include <math.h>

#define epsilon 1e-4

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

typedef float CG2DVertex[2];
typedef int CG2DEdge[2];
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

bool isSameEdge(const CG2DEdge * const edge1,
                const CG2DEdge * const edge2) {
  bool b = ((*edge1)[0] == (*edge2)[0] && (*edge1)[1] == (*edge2)[1])
           || ((*edge1)[0] == (*edge2)[1] && (*edge1)[1] == (*edge2)[0]);
  return b;
}

bool edgeInTriangle(const CG2DEdge *const edge,
                    const struct Triangle *const triangle) {
  const CG2DEdge edges[3] = {
    {triangle->indices[0], triangle->indices[1]},
    {triangle->indices[1], triangle->indices[2]},
    {triangle->indices[2], triangle->indices[0]},
  };
  return isSameEdge(edge, &(edges[0])) || isSameEdge(edge, &(edges[1]))
         || isSameEdge(edge, &(edges[2]));
}

#define vec_cross(o, p1, p2) \
  (((p1)[0] - (o)[0]) * ((p2)[1] - (o)[1]) - ((p1)[1] - (o)[1]) * ((p2)[0] - (o)[0]))
bool intersectedSegment(const CG2DVertex *vertices, const CG2DEdge l1, const CG2DEdge l2) {
  float AC_AD = vec_cross(vertices[l1[0]], vertices[l2[0]], vertices[l2[1]]);
  float BC_BD = vec_cross(vertices[l1[1]], vertices[l2[0]], vertices[l2[1]]);
  float CA_CB = vec_cross(vertices[l2[0]], vertices[l1[0]], vertices[l1[1]]);
  float DA_DB = vec_cross(vertices[l2[1]], vertices[l1[0]], vertices[l1[1]]);
  return AC_AD * BC_BD <= 0 && CA_CB * DA_DB <= 0;
}

float triangleArea(const struct Triangle *triangle) {
#define x(i) (triangle->vertices[i][AXIS_X])
#define y(i) (triangle->vertices[i][AXIS_Y])
#define pm(i) (((i) + 1) % 3)
#define cross(i) (x(i) * y(pm(i)) - x(pm(i)) * y(i))
  return (cross(0) + cross(1) + cross(2)) / 2;
#undef cross
#undef pm
#undef x
#undef y
}

#define x(i) (triangle->vertices[(i) - 1][AXIS_X])
#define y(i) (triangle->vertices[(i) - 1][AXIS_Y])
void getCircumscribedCircle(
    const struct Triangle *const triangle,
    struct Circle *circle) {
  const float A1 = 2 * (x(2) - x(1));
  const float A2 = 2 * (x(3) - x(2));
  const float B1 = 2 * (y(2) - y(1));
  const float B2 = 2 * (y(3) - y(2));
  const float C1 = square_diff(x(2), x(1)) + square_diff(y(2), y(1));
  const float C2 = square_diff(x(3), x(2)) + square_diff(y(3), y(2));
  const float delta = A1 * B2 - A2 * B1;
  if (fabsf(delta) < epsilon) {
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

#define releaseArray(_array)      \
  do {                            \
    Array_reset(_array, nullptr); \
    Array_destroy(_array);        \
  } while (false)

inline Array *getBadTriangleArray(const Array *const triangle_array,
                                  const CG2DVertex vert,
                                  const Allocator *const allocator) {
  Array *const bad_triangles = Array_new(sizeof(struct Triangle), allocator);
  const int n_triangles = (int) Array_length(triangle_array);
  struct Triangle *const triangles = Array_get(triangle_array, 0);
  for (int j = 0; j < n_triangles; j++) {
    struct Triangle *triangle = &triangles[j];
    struct Circle circle = {};
    getCircumscribedCircle(triangle, &circle);
    if (vert_distance(vert, circle.center) < circle.radius) {
      // the vert is in the circle,
      // means the triangle is not a Delaunay triangle
      triangle->isBad = true;
      Array_append(bad_triangles, triangle, 1);
    }
  }
  return bad_triangles;
}

bool vertAtLeftOfSegment(const CG2DVertex seg_verts[2],
                         const CG2DVertex vert) {
  if (seg_verts[0][AXIS_Y] == seg_verts[1][AXIS_Y]) {
    return false;
  }
  const float min_y = min(seg_verts[0][AXIS_Y], seg_verts[1][AXIS_Y]);
  const float max_y = max(seg_verts[0][AXIS_Y], seg_verts[1][AXIS_Y]);
  if (vert[AXIS_Y] <= min_y || vert[AXIS_Y] >= max_y) {
    return false;
  }
  const float y_y1 = vert[AXIS_Y] - seg_verts[0][AXIS_Y];
  const float y2_y1 = seg_verts[1][AXIS_Y] - seg_verts[0][AXIS_Y];
  const float x_x1 = vert[AXIS_X] - seg_verts[0][AXIS_X];
  const float x2_x1 = seg_verts[1][AXIS_X] - seg_verts[0][AXIS_X];
  return y_y1 / y2_y1 * x2_x1 > x_x1;
}

bool vertInPolygon(const Array *vert_array,
                   const CG2DVertex vert) {
  int count = 0;
  const int n_verts = (int) Array_length(vert_array);
  const CG2DVertex *const vertices = Array_get(vert_array, 0);
  for (int i = 0; i < n_verts - 1; i++) {
    const CG2DVertex *const edge_verts = &vertices[i];
    count += vertAtLeftOfSegment(edge_verts, vert);
  }
  return count & 1;
}

bool vertInTriangle(const float angle_verts[3][2],
                    const CG2DVertex vert) {
  const float s = vec_cross(angle_verts[1], angle_verts[0], angle_verts[2]);
  const float s1 = vec_cross(angle_verts[1], angle_verts[0], vert);
  const float s2 = vec_cross(angle_verts[1], angle_verts[2], vert);
  return 0 < s1 - s2 && s1 - s2 < s - epsilon;
}

typedef struct vertNdxInfo {
  int left;
  int right;
  int index;
  bool isConvex;
  bool outOfCount;
  int nInnerVert;
} VNI;

struct SharedEdge {
  CG2DEdge edge;
  int triangles[2];
};

struct SharedEdge *findEdge(Array *edge_array, const CG2DEdge *edge);
bool isPositiveAngle(const CG2DVertex *vertices, const VNI *vni);
Array *buildVniAndIncArray(const Array *vert_array,
                           Array *inc_arrays,
                           const Allocator *allocator);
bool isEarVNI(const VNI *vni);
VNI *findEarVNI(VNI *vnies, int count);
int oppositeVert(struct Triangle *pTriangle, const CG2DEdge edge);

struct SharedEdge *findEdge(Array *edge_array, const CG2DEdge * edge) {
  const int count = (int) Array_length(edge_array);
  struct SharedEdge * const edges = Array_get(edge_array, 0);
  for (int i = 0; i < count; i ++) {
    if (isSameEdge(edge, &edges[i].edge)) {
      return &edges[i];
    }
  }
  return NULL;
}

#define angle_cross(angle_verts) \
(((angle_verts)[1][AXIS_X] - (angle_verts)[0][AXIS_X]) * \
 ((angle_verts)[2][AXIS_Y] - (angle_verts)[1][AXIS_Y]) - \
 ((angle_verts)[1][AXIS_Y] - (angle_verts)[0][AXIS_Y]) * \
 ((angle_verts)[2][AXIS_X] - (angle_verts)[1][AXIS_X]))
bool isPositiveAngle(const CG2DVertex * const vertices, const VNI * vni) {
  const CG2DVertex angle_verts[3] = {
      {vertices[vni->left][AXIS_X], vertices[vni->left][AXIS_Y]},
      {vertices[vni->index][AXIS_X], vertices[vni->index][AXIS_Y]},
      {vertices[vni->right][AXIS_X], vertices[vni->right][AXIS_Y]},
  };
  return angle_cross(angle_verts) > 0;
}

#define arrays_get(arrays, ndx) ((Array *)(((void *)inc_arrays) + (ndx) * sizeof_array))

Array *buildVniAndIncArray(const Array *const vert_array,
                           Array *const inc_arrays,
                           const Allocator *const allocator) {
  const int n_verts = (int) Array_length(vert_array);
  if (n_verts < 3) { return nullptr; }

  Array *pVNI_array = Array_new(sizeof(VNI), allocator);
  const CG2DVertex *const vertices = Array_get(vert_array, 0);
  for (int ndx = 0; ndx < n_verts; ndx ++) {
    const int v1 = (ndx + n_verts - 1) % n_verts;
    const int v2 = (ndx + n_verts + 1) % n_verts;
    const CG2DVertex *const angle = &vertices[v1];
    VNI vni = {};
    vni.left = v1;
    vni.right = v2;
    vni.index = ndx;
    vni.isConvex = isPositiveAngle(vertices, &vni);
    vni.outOfCount = false;
    vni.nInnerVert = 0;
    for (int i = 0; i < n_verts; i++) {
      if (v1 <= i && i <= v2) { continue; }
      if (vertInTriangle(angle, vertices[i])) {
        vni.nInnerVert++;
        Array * n_inc_array = arrays_get(inc_arrays, i);
        Array_append(n_inc_array, &ndx, 1);
      }
    }
    Array_append(pVNI_array, &vni, 1);
  }
  return pVNI_array;
}

inline bool isEarVNI(const VNI * vni) {
  return vni->isConvex && vni->nInnerVert == 0;
}

inline VNI * findEarVNI(VNI * const vnies, const int count) {
  for (int i = 0; i < count; i++) {
    VNI *vni = &vnies[i];
    if (!vni->outOfCount && isEarVNI(vni)) {
      vni->outOfCount = true;
      return vni;
    }
  }
  return nullptr;
}

int oppositeVert(struct Triangle *pTriangle, const CG2DEdge edge) {
  for (int i = 0; i < 3; i++) {
    if (pTriangle->indices[i] != edge[0]
     && pTriangle->indices[i] != edge[1]) {
      return i;
    }
  }
  return -1;
}

bool legalizeTriangulation(struct Triangle * const triangles,
                           Array * edge_array) {
  bool flipped = false;
  const int n_edges = (int) Array_length(edge_array);
  struct SharedEdge * edges = Array_get(edge_array, 0);
  for (int i = 0; i < n_edges; i ++) {
    if (edges[i].triangles[1] < 0) { continue; }
    struct Triangle * triangle1 = &triangles[edges[i].triangles[0]];
    struct Triangle * triangle2 = &triangles[edges[i].triangles[1]];
    int vert1 = oppositeVert(triangle1, edges[i].edge);
    int vert2 = oppositeVert(triangle2, edges[i].edge);
    struct Circle circle1 = {};
    getCircumscribedCircle(triangle1, &circle1);
    if (vert_distance(triangle2->vertices[vert2], circle1.center) < circle1.radius - epsilon) {
#define con (triangle1->indices[(vert1 + 1) % 3] == triangle2->indices[(vert2 + 1) % 3])
      int ndx1 = (vert1 + 1) % 3;
      int ndx2 = (vert2 + 1 + con) % 3;
      triangle1->indices[ndx1] = triangle2->indices[vert2];
      triangle1->vertices[ndx1][AXIS_X] = triangle2->vertices[vert2][AXIS_X];
      triangle1->vertices[ndx1][AXIS_Y] = triangle2->vertices[vert2][AXIS_Y];
      triangle2->indices[ndx2] = triangle1->indices[vert1];
      triangle2->vertices[ndx2][AXIS_X] = triangle1->vertices[vert1][AXIS_X];
      triangle2->vertices[ndx2][AXIS_Y] = triangle1->vertices[vert1][AXIS_Y];
      edges[i].edge[0] = triangle1->indices[vert1];
      edges[i].edge[1] = triangle2->indices[vert2];
      flipped = true;
    }
  }
  return flipped;
}

#define populateTriangleFromEarVNI(triangle) do { \
  (triangle)->vertices[0][AXIS_X] = vertices[ear_vni->left][AXIS_X]; \
  (triangle)->vertices[0][AXIS_Y] = vertices[ear_vni->left][AXIS_Y]; \
  (triangle)->vertices[1][AXIS_X] = vertices[ear_vni->index][AXIS_X]; \
  (triangle)->vertices[1][AXIS_Y] = vertices[ear_vni->index][AXIS_Y]; \
  (triangle)->vertices[2][AXIS_X] = vertices[ear_vni->right][AXIS_X]; \
  (triangle)->vertices[2][AXIS_Y] = vertices[ear_vni->right][AXIS_Y]; \
  (triangle)->indices[0] = ear_vni->left; \
  (triangle)->indices[1] = ear_vni->index; \
  (triangle)->indices[2] = ear_vni->right; \
  (triangle)->isBad = false; \
  (triangle)->isExterior = false; \
} while (false)
Array *xglEarClippingTriangulate(const Array *const vert_array, const Allocator *allocator) {
  const int count = (int) Array_length(vert_array);
  const CG2DVertex *const vertices = Array_get(vert_array, 0);

  // arrays for every vertex that records those angle the vertex in.
  Array *inc_arrays = allocator->calloc(count, sizeof_array);
  for (int i = 0; i < count; i++) {
    Array_init(arrays_get(inc_arrays, i), sizeof(int), allocator);
  }
  // build vni and include array
  Array * const pVNI_array = buildVniAndIncArray(vert_array, inc_arrays, allocator);

  // allocate triangles
  struct Triangle * const triangles = allocator->calloc(count - 2, sizeof(struct Triangle));
  Array * edge_array = Array_new(sizeof(struct SharedEdge), allocator);

  VNI *const vnies = Array_get(pVNI_array, 0);
  int n_triangles = 0;
  VNI *ear_vni = findEarVNI(vnies, count);
  while (ear_vni) {
    // populate triangle
    populateTriangleFromEarVNI(&triangles[n_triangles]);
    const struct Triangle *triangle = &triangles[n_triangles];
    for (int i = 0; i < 3; i++) {
      const CG2DEdge edge = {triangle->indices[i], triangle->indices[(i + 1) % 3]};
      struct SharedEdge *pse = findEdge(edge_array, &edge);
      if (pse) {
        pse->triangles[1] = n_triangles;
      } else {
        struct SharedEdge se = {};
        se.edge[0] = edge[0];
        se.edge[1] = edge[1];
        se.triangles[0] = n_triangles;
        se.triangles[1] = -1;
        Array_append(edge_array, &se, 1);
      }
    }
    n_triangles++;

    // connect left and right
    VNI * left = &vnies[ear_vni->left];
    VNI * right = &vnies[ear_vni->right];
    left->right = ear_vni->right;
    right->left = ear_vni->left;
    // update convexity of left and right
    left->isConvex = isPositiveAngle(vertices, left);
    right->isConvex = isPositiveAngle(vertices, right);

    // update number of inner vertices for angles those include current vertex
    const Array * const inc_array = arrays_get(inc_arrays, ear_vni->index);
    const int * const indices = Array_get(inc_array, 0);
    const int n_indices = (int) Array_length(inc_array);
    for (int i = 0; i < n_indices; i ++) {
      vnies[indices[i]].nInnerVert --;
    }

    // find other ear vertex
    ear_vni = findEarVNI(vnies, count);
  }
  while(legalizeTriangulation(triangles, edge_array));
  releaseArray(edge_array);

  Array *index_array = Array_new(sizeof(int), allocator);
  for (int i = 0; i < n_triangles; i++) {
    const struct Triangle *triangle = &triangles[i];
    Array_append(index_array, &(triangle->indices), 3);
  }
  allocator->free(triangles);

  releaseArray(pVNI_array);
  for (int i = 0; i < count; i++) {
    Array_reset(arrays_get(inc_arrays, i), nullptr);
  }
  allocator->free(inc_arrays);

  return index_array;
}
