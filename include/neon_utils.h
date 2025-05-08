/**
 * neon_utils.h
 * Utility functions and macros for ARM NEON SIMD programming
 */
#ifndef NEON_UTILS_H
#define NEON_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "platform_detect.h"

#ifdef HAS_NEON
#include <arm_neon.h>
#else
#error "ARM NEON support is required for this project"
#endif

/**
 * Alignment macros for NEON-friendly memory allocation
 */
#define NEON_ALIGNMENT 16
#define NEON_ALIGN __attribute__((aligned(NEON_ALIGNMENT)))
#define IS_ALIGNED(PTR, ALIGN) (((uintptr_t)(PTR) & ((ALIGN) - 1)) == 0)

/**
 * Memory allocation helpers that ensure proper alignment for NEON operations
 */
static inline void* neon_malloc(size_t size) {
    void* ptr;
    int result = posix_memalign(&ptr, NEON_ALIGNMENT, size);
    if (result != 0) {
        return NULL;
    }
    return ptr;
}

/**
 * Helper functions to print vector contents for debugging
 */
static inline void print_float32x4(const char* name, float32x4_t v) {
    float values[4];
    vst1q_f32(values, v);
    printf("%s = [%.6f, %.6f, %.6f, %.6f]\n", 
           name, values[0], values[1], values[2], values[3]);
}

static inline void print_int32x4(const char* name, int32x4_t v) {
    int32_t values[4];
    vst1q_s32(values, v);
    printf("%s = [%d, %d, %d, %d]\n", 
           name, values[0], values[1], values[2], values[3]);
}

static inline void print_uint32x4(const char* name, uint32x4_t v) {
    uint32_t values[4];
    vst1q_u32(values, v);
    printf("%s = [%u, %u, %u, %u]\n", 
           name, values[0], values[1], values[2], values[3]);
}

static inline void print_float32x2(const char* name, float32x2_t v) {
    float values[2];
    vst1_f32(values, v);
    printf("%s = [%.6f, %.6f]\n", name, values[0], values[1]);
}

static inline void print_int16x8(const char* name, int16x8_t v) {
    int16_t values[8];
    vst1q_s16(values, v);
    printf("%s = [%d, %d, %d, %d, %d, %d, %d, %d]\n", 
           name, values[0], values[1], values[2], values[3],
           values[4], values[5], values[6], values[7]);
}

/**
 * Function to check if the current CPU supports ARM NEON
 * Note: This is always true on ARMv8 (AArch64) but included for clarity
 */
static inline int check_neon_support(void) {
#ifdef HAS_NEON
    return 1;
#else
    return 0;
#endif
}

#endif /* NEON_UTILS_H */
