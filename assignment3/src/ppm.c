#include "ppm.h"

// --- https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}
// ---

char* generate_intensity_scheme(size_t n_levels) {
    char* scheme = malloc(n_levels * sizeof(char) * 12);

    for (size_t i = 0; i < n_levels; ++i) {
        sprintf(scheme + i * 12, "%3ld %3ld %3ld\n", i, i, i);
    }

    return scheme;
}

char* generate_attractor_scheme(size_t n_colors) {
    char* scheme = malloc(n_colors * sizeof(char) * 12);

    hsv color;
    rgb color_out;
    color.s = 1.0;
    color.v = 1.0;
    for (size_t i = 0; i < n_colors; ++i) {
        color.h = (360.0 * i) / ((float) n_colors);
        color_out = hsv2rgb(color);
        sprintf(
            scheme + i * 12, "%3d %3d %3d\n",
            (int) (color_out.r * 255),
            (int) (color_out.g * 255),
            (int) (color_out.b * 255)
        );
    }

    return scheme;
}

void write_header(FILE* fp, int dims, int color_max) {
    fprintf(fp, "P3\n%d %d\n%d\n", dims, dims, color_max);
}

void write_row(FILE* fp, size_t color_index, char* scheme) {
    fwrite(scheme + color_index * 12, sizeof(char), 12, fp);
}
