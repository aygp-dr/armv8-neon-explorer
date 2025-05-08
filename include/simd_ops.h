/**
 * simd_ops.h
 * Core interfaces for ARM NEON SIMD operations
 */
#ifndef SIMD_OPS_H
#define SIMD_OPS_H

#include <stdint.h>
#include <stddef.h>
#include "platform_detect.h"
#include "neon_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Vector Addition Functions
 * C = A + B
 */

// Float vector addition (32-bit float elements)
void simd_add_f32(const float* a, const float* b, float* c, size_t len);

// Integer vector addition (32-bit signed elements)
void simd_add_s32(const int32_t* a, const int32_t* b, int32_t* c, size_t len);

// Integer vector addition (16-bit signed elements)
void simd_add_s16(const int16_t* a, const int16_t* b, int16_t* c, size_t len);

// Integer vector addition (8-bit unsigned elements)
void simd_add_u8(const uint8_t* a, const uint8_t* b, uint8_t* c, size_t len);

/**
 * Vector Multiplication Functions
 * C = A * B (element-wise)
 */

// Float vector multiplication (32-bit float elements)
void simd_mul_f32(const float* a, const float* b, float* c, size_t len);

// Integer vector multiplication (32-bit signed elements)
void simd_mul_s32(const int32_t* a, const int32_t* b, int32_t* c, size_t len);

// Integer vector multiplication (16-bit signed elements)
void simd_mul_s16(const int16_t* a, const int16_t* b, int16_t* c, size_t len);

/**
 * Vector Dot Product Functions
 * result = sum(A[i] * B[i])
 */

// Float vector dot product (32-bit float elements)
float simd_dot_product_f32(const float* a, const float* b, size_t len);

// Integer vector dot product (32-bit signed elements)
int32_t simd_dot_product_s32(const int32_t* a, const int32_t* b, size_t len);

/**
 * Vector Comparison Operations
 * Returns a mask where each element is all 1s (if true) or all 0s (if false)
 */

// C = (A > B) ? 0xFFFFFFFF : 0x00000000
void simd_cmpgt_f32(const float* a, const float* b, uint32_t* result, size_t len);

// C = (A == B) ? 0xFFFFFFFF : 0x00000000
void simd_cmpeq_f32(const float* a, const float* b, uint32_t* result, size_t len);

/**
 * Vector Maximum/Minimum Functions
 * C = max(A, B) or C = min(A, B) element-wise
 */

// C = max(A, B) for float
void simd_max_f32(const float* a, const float* b, float* c, size_t len);

// C = min(A, B) for float
void simd_min_f32(const float* a, const float* b, float* c, size_t len);

/**
 * Other Vector Operations
 */

// Absolute value: C = abs(A)
void simd_abs_f32(const float* a, float* c, size_t len);

// Square root: C = sqrt(A)
void simd_sqrt_f32(const float* a, float* c, size_t len);

// Interleave even elements from A and B: C = [A[0], B[0], A[2], B[2], ...]
void simd_interleave_even_f32(const float* a, const float* b, float* c, size_t len);

// Interleave odd elements from A and B: C = [A[1], B[1], A[3], B[3], ...]
void simd_interleave_odd_f32(const float* a, const float* b, float* c, size_t len);

/**
 * Image Processing Operations
 */

// Grayscale conversion: output = 0.299*R + 0.587*G + 0.114*B
void simd_rgb_to_gray(const uint8_t* rgb, uint8_t* gray, size_t pixel_count);

// Simple box blur (3x3) for grayscale image
void simd_blur_gray_3x3(const uint8_t* input, uint8_t* output, int width, int height);

#ifdef __cplusplus
}
#endif

#endif /* SIMD_OPS_H */
