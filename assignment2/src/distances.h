#ifndef DISTANCES
#define DISTANCES

#include <math.h>
#include <x86intrin.h>
#include <stdint.h>

#include "point.h"

#define MAX_SQ 20000*20000 * 3
#define DIST_FUNC calc_dist_intrin

static inline int calc_dist(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;
    return sqrt(dist_sq);
}

static inline int calc_dist_intrin(Point* p1, Point* p2) {

    // Load the points into vector registers
    int32_t _p1[4] = {p1->x, p1->y, p1->z, 0};
    int32_t _p2[4] = {p2->x, p2->y, p2->z, 0};
    int32_t _dxdydz_sq[4];

    __m128i _m_p1 = _mm_loadu_si128((__m128i*) _p1);
    __m128i _m_p2 = _mm_loadu_si128((__m128i*) _p2);

    // Perform vector calculations
    __m128i _m_dxdydz = _mm_sub_epi32(_m_p2, _m_p1);
    __m128i _m_dxdydz_sq = _mm_mullo_epi32(_m_dxdydz, _m_dxdydz);

    _mm_storeu_si128((__m128i*) _dxdydz_sq, _m_dxdydz_sq);

    int dist_sq = _dxdydz_sq[0] + _dxdydz_sq[1] + _dxdydz_sq[2];
    return sqrt(dist_sq);
}

static inline int calc_dist_intrin_dot(PointFloat* p1, PointFloat* p2) {

    // Load the points into vector registers
    float _p1[4] = {p1->x, p1->y, p1->z, 0.0f};
    float _p2[4] = {p2->x, p2->y, p2->z, 0.0f};
    float dist_sq[4];

    __m128 _m_p1 = _mm_loadu_ps(_p1);
    __m128 _m_p2 = _mm_loadu_ps(_p2);

    // Perform vector calculations
    __m128 _m_dist = _mm_sub_ps(_m_p2, _m_p1);
    __m128 _m_dot = _mm_dp_ps(_m_dist, _m_dist, 0xff);

    _mm_storeu_ps(dist_sq, _m_dot);

    return (int)sqrt(dist_sq[0]);
}

static inline int calc_dist_float(Point* p1, Point* p2) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float dz = p2->z - p1->z;

    float dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

static inline int calc_dist_float_point(PointFloat* p1, PointFloat* p2) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float dz = p2->z - p1->z;

    float dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

static inline int calc_dist_double(Point* p1, Point* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dz = p2->z - p1->z;

    double dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

static inline int calc_dist_double_point(PointDouble* p1, PointDouble* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dz = p2->z - p1->z;

    double dist_sq = dx*dx + dy*dy + dz*dz;
    return (int) sqrt(dist_sq);
}

static inline int calc_dist_binary_search(Point* p1, Point* p2) {
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int dz = p2->z - p1->z;

    int dist_sq = dx*dx + dy*dy + dz*dz;

    int low = 0;
    int high = MAX_SQ;

    while (low < high) {
        int m = (low + high) / 2;
        int sq = m*m;
        if ( sq == dist_sq ) return m;
        else if (sq > dist_sq) high = m - 1;
        else low = m + 1;
    }
    return low;
}

static inline void increment_bin(int dist, int* bins) {
    // Increment the correct bin
    int bin = (dist + 5) / 10; // Truncate to 4 digits
    bins[bin] += 1;
}

static inline void distance(Point* p1, Point* p2, int* bins) {
    // Calculate distance between point p1 and p2
    // and increase the bin with that distance
    int dist = DIST_FUNC(p1, p2);
    increment_bin(dist, bins);
}

#endif
