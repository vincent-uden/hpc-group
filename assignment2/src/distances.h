#ifndef DISTANCES
#define DISTANCES

#include <math.h>
#include <x86intrin.h>
#include <stdint.h>

#include "point.h"

#define MAX_SQ 20000*20000 * 3
#define DIST_FUNC calc_dist

static inline int calc_dist(Point* p1, Point* p2);
static inline int calc_dist_intrin(Point* p1, Point* p2);
static inline int calc_dist_intrin_dot(PointFloat* p1, PointFloat* p2);
static inline int calc_dist_float(Point* p1, Point* p2);
static inline int calc_dist_float_point(PointFloat* p1, PointFloat* p2);
static inline int calc_dist_double(Point* p1, Point* p2);
static inline int calc_dist_double_point(PointDouble* p1, PointDouble* p2);
static inline int calc_dist_binary_search(Point* p1, Point* p2);
static inline void distance(Point* p1, Point* p2, int* bins);

#endif
