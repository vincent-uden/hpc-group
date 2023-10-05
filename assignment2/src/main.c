#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "cli.h"
#include "point.h"
#include "read_file.h"
#include "distances.h"

#define BINS 3465


static inline void inc_bin(int32_t* dist_squared, int* bins) {
    int dist = sqrt(dist_squared[0] + dist_squared[1] + dist_squared[2]);
    int bin = (dist + 5) / 10; // Truncate to 4 digits
    bins[bin] += 1;
}

static inline void packed_dist(Point* point_buffer_1, Point* point_buffer_2, size_t i, size_t j, int* bins, int thread_num) {
    int32_t* _p1 = (int32_t*) point_buffer_1 + i;

    int32_t* _p2 = (int32_t*) point_buffer_2 + j;
    int32_t* _p3 = (int32_t*) point_buffer_2 + j + 1;
    int32_t* _p4 = (int32_t*) point_buffer_2 + j + 2;
    int32_t* _p5 = (int32_t*) point_buffer_2 + j + 3;

    int32_t _dxdydz_sq2[4];
    int32_t _dxdydz_sq3[4];
    int32_t _dxdydz_sq4[4];
    int32_t _dxdydz_sq5[4];

    __m128i _m_p1 = _mm_loadu_si128((__m128i*) _p1);
    __m128i _m_p2 = _mm_loadu_si128((__m128i*) _p2);
    __m128i _m_dxdydz2 = _mm_sub_epi32(_m_p2, _m_p1);

    _m_p1 = _mm_loadu_si128((__m128i*) _p1);
    __m128i _m_p3 = _mm_loadu_si128((__m128i*) _p3);
    __m128i _m_dxdydz3 = _mm_sub_epi32(_m_p3, _m_p1);

    _m_p1 = _mm_loadu_si128((__m128i*) _p1);
    __m128i _m_p4 = _mm_loadu_si128((__m128i*) _p4);
    __m128i _m_dxdydz4 = _mm_sub_epi32(_m_p4, _m_p1);

    _m_p1 = _mm_loadu_si128((__m128i*) _p1);
    __m128i _m_p5 = _mm_loadu_si128((__m128i*) _p5);
    __m128i _m_dxdydz5 = _mm_sub_epi32(_m_p5, _m_p1);

    __m128i _m_dxdydz_sq2 = _mm_mullo_epi32(_m_dxdydz2, _m_dxdydz2);
    _mm_storeu_si128((__m128i*) _dxdydz_sq2, _m_dxdydz_sq2);

    __m128i _m_dxdydz_sq3 = _mm_mullo_epi32(_m_dxdydz3, _m_dxdydz3);
    _mm_storeu_si128((__m128i*) _dxdydz_sq3, _m_dxdydz_sq3);

    __m128i _m_dxdydz_sq4 = _mm_mullo_epi32(_m_dxdydz4, _m_dxdydz4);
    _mm_storeu_si128((__m128i*) _dxdydz_sq4, _m_dxdydz_sq4);

    __m128i _m_dxdydz_sq5 = _mm_mullo_epi32(_m_dxdydz5, _m_dxdydz5);
    _mm_storeu_si128((__m128i*) _dxdydz_sq5, _m_dxdydz_sq5);

    // All bins at the end is faster
    inc_bin(_dxdydz_sq2, bins + thread_num * BINS);
    inc_bin(_dxdydz_sq3, bins + thread_num * BINS);
    inc_bin(_dxdydz_sq4, bins + thread_num * BINS);
    inc_bin(_dxdydz_sq5, bins + thread_num * BINS);
}

int main(int argc, char** argv) {

    CliArgs args = parse_cli(argc, argv);

    omp_set_num_threads(args.threads);
    if (args.verbose) {
        printf("Attempting to use %zu threads\n", args.threads);
    }

    FILE* fp = fopen(args.input_file, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s\n", args.input_file);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    size_t n_chunks = (file_size - 1) / (ROW_LEN * args.chunk_size) + 1;

    Point* point_buffer_1 = malloc(sizeof(Point) * args.chunk_size);
    Point* point_buffer_2 = malloc(sizeof(Point) * args.chunk_size);
    char* read_buffer = malloc(ROW_LEN * args.chunk_size);

    int* bins = calloc(sizeof(int), BINS*args.threads);

    if (point_buffer_1 == NULL || point_buffer_2 == NULL || read_buffer == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        exit(1);
    }

    size_t rows_read;

    for (size_t c1 = 0; c1 < n_chunks; ++c1) {
        rows_read = read_chunk(fp, c1 * args.chunk_size, args.chunk_size, point_buffer_1, read_buffer);

        #pragma omp parallel for
        for (size_t i = 0; i < rows_read; ++i) {
            for (size_t j = i + 1; j + 3 < rows_read; j+=4) {
                if (args.verbose) {
                    printf("%zu %zu\n", i, j);
                    printf("%zu %zu\n", i, j+1);
                    printf("%zu %zu\n", i, j+2);
                    printf("%zu %zu\n", i, j+3);
                }
                packed_dist(point_buffer_1, point_buffer_2, i, j, bins, omp_get_thread_num());
            }
        }

        for (size_t c2 = c1 + 1; c2 < n_chunks; ++c2) {
            rows_read = read_chunk(fp, c2 * args.chunk_size, args.chunk_size, point_buffer_2, read_buffer);

            #pragma omp parallel for
            for (size_t i = 0; i < args.chunk_size; ++i) {
                for (size_t j = 0; j < rows_read; j += 4) {
                    if (args.verbose) {
                        printf("%zu %zu\n", i, j);
                        printf("%zu %zu\n", i, j+1);
                        printf("%zu %zu\n", i, j+2);
                        printf("%zu %zu\n", i, j+3);
                    }
                    packed_dist(point_buffer_1, point_buffer_2, i, j, bins, omp_get_thread_num());
                }
            }
        }
    }

    free(point_buffer_1);
    free(point_buffer_2);
    free(read_buffer);
    fclose(fp);

    int x;
    for (size_t i = 0; i < BINS; ++i) {
        x = 0;
        for ( int t = 0; t < args.threads; t++ ) x += bins[i+t*BINS];
        if (x > 0) {
            printf("%02zu.%02zu %d\n", i / 100, i % 100, x);
        }
    }

    return 0;
}
