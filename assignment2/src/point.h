#ifndef POINT
#define POINT

typedef struct {
    int x;
    int y;
    int z;
    int pad;
} Point;

typedef struct {
    float x;
    float y;
    float z;
} PointFloat;

typedef struct {
    double x;
    double y;
    double z;
} PointDouble;

#endif
