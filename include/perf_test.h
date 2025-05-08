/**
 * perf_test.h
 * Performance testing utilities for SIMD operations
 */
#ifndef PERF_TEST_H
#define PERF_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
 * Timing functions
 */
static inline uint64_t get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

typedef struct {
    const char* name;
    uint64_t start_time;
    uint64_t total_time;
    uint64_t calls;
} perf_timer_t;

static inline void timer_start(perf_timer_t* timer) {
    timer->start_time = get_time_us();
    timer->calls++;
}

static inline void timer_stop(perf_timer_t* timer) {
    uint64_t end_time = get_time_us();
    timer->total_time += (end_time - timer->start_time);
}

static inline void timer_reset(perf_timer_t* timer) {
    timer->total_time = 0;
    timer->calls = 0;
}

static inline void timer_print(perf_timer_t* timer) {
    printf("%-20s: %9lu us total, %6lu calls, %9.2f us/call\n",
           timer->name,
           (unsigned long)timer->total_time,
           (unsigned long)timer->calls,
           timer->calls > 0 ? (double)timer->total_time / timer->calls : 0.0);
}

/**
 * Initialize a new performance timer
 */
static inline perf_timer_t* timer_create(const char* name) {
    perf_timer_t* timer = (perf_timer_t*)malloc(sizeof(perf_timer_t));
    if (timer) {
        timer->name = name;
        timer->start_time = 0;
        timer->total_time = 0;
        timer->calls = 0;
    }
    return timer;
}

/**
 * Free a performance timer
 */
static inline void timer_destroy(perf_timer_t* timer) {
    free(timer);
}

/**
 * Comparison timing framework - compares SIMD vs non-SIMD implementations
 */
typedef struct {
    const char* name;
    perf_timer_t* simd_timer;
    perf_timer_t* scalar_timer;
    double speedup;
} perf_comparison_t;

static inline perf_comparison_t* comparison_create(const char* name) {
    perf_comparison_t* comp = (perf_comparison_t*)malloc(sizeof(perf_comparison_t));
    if (comp) {
        comp->name = name;
        comp->simd_timer = timer_create("SIMD");
        comp->scalar_timer = timer_create("Scalar");
        comp->speedup = 0.0;
    }
    return comp;
}

static inline void comparison_calculate(perf_comparison_t* comp) {
    if (comp->scalar_timer->total_time > 0) {
        comp->speedup = (double)comp->scalar_timer->total_time / comp->simd_timer->total_time;
    } else {
        comp->speedup = 0.0;
    }
}

static inline void comparison_print(perf_comparison_t* comp) {
    printf("\n=== %s Performance Comparison ===\n", comp->name);
    timer_print(comp->simd_timer);
    timer_print(comp->scalar_timer);
    comparison_calculate(comp);
    printf("Speedup: %.2fx\n\n", comp->speedup);
}

static inline void comparison_destroy(perf_comparison_t* comp) {
    timer_destroy(comp->simd_timer);
    timer_destroy(comp->scalar_timer);
    free(comp);
}

/**
 * Generate random data for testing
 */
static inline void fill_random_float(float* array, size_t len, float min_val, float max_val) {
    for (size_t i = 0; i < len; i++) {
        float r = (float)rand() / RAND_MAX;
        array[i] = min_val + r * (max_val - min_val);
    }
}

static inline void fill_random_int32(int32_t* array, size_t len, int32_t min_val, int32_t max_val) {
    for (size_t i = 0; i < len; i++) {
        float r = (float)rand() / RAND_MAX;
        array[i] = min_val + (int32_t)(r * (max_val - min_val));
    }
}

static inline void fill_random_uint8(uint8_t* array, size_t len) {
    for (size_t i = 0; i < len; i++) {
        array[i] = (uint8_t)(rand() & 0xFF);
    }
}

#endif /* PERF_TEST_H */
