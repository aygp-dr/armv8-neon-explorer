/**
 * simd_ops.c
 * Implementation of ARM NEON SIMD operations
 */
#include "simd_ops.h"
#include <arm_neon.h>

/* 
 * Vector Addition Functions
 */

void simd_add_f32(const float* a, const float* b, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector addition
        float32x4_t vc = vaddq_f32(va, vb);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

void simd_add_s32(const int32_t* a, const int32_t* b, int32_t* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 int32 elements from each array
        int32x4_t va = vld1q_s32(a + i * 4);
        int32x4_t vb = vld1q_s32(b + i * 4);
        
        // Perform vector addition
        int32x4_t vc = vaddq_s32(va, vb);
        
        // Store the result
        vst1q_s32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

void simd_add_s16(const int16_t* a, const int16_t* b, int16_t* c, size_t len) {
    // Process 8 elements at a time using NEON
    size_t vec_size = len / 8;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 8 int16 elements from each array
        int16x8_t va = vld1q_s16(a + i * 8);
        int16x8_t vb = vld1q_s16(b + i * 8);
        
        // Perform vector addition
        int16x8_t vc = vaddq_s16(va, vb);
        
        // Store the result
        vst1q_s16(c + i * 8, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 8; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

void simd_add_u8(const uint8_t* a, const uint8_t* b, uint8_t* c, size_t len) {
    // Process 16 elements at a time using NEON
    size_t vec_size = len / 16;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 16 uint8 elements from each array
        uint8x16_t va = vld1q_u8(a + i * 16);
        uint8x16_t vb = vld1q_u8(b + i * 16);
        
        // Perform vector addition
        uint8x16_t vc = vaddq_u8(va, vb);
        
        // Store the result
        vst1q_u8(c + i * 16, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 16; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

/* 
 * Vector Multiplication Functions
 */

void simd_mul_f32(const float* a, const float* b, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector multiplication
        float32x4_t vc = vmulq_f32(va, vb);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = a[i] * b[i];
    }
}

void simd_mul_s32(const int32_t* a, const int32_t* b, int32_t* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 int32 elements from each array
        int32x4_t va = vld1q_s32(a + i * 4);
        int32x4_t vb = vld1q_s32(b + i * 4);
        
        // Perform vector multiplication
        int32x4_t vc = vmulq_s32(va, vb);
        
        // Store the result
        vst1q_s32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = a[i] * b[i];
    }
}

void simd_mul_s16(const int16_t* a, const int16_t* b, int16_t* c, size_t len) {
    // Process 8 elements at a time using NEON
    size_t vec_size = len / 8;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 8 int16 elements from each array
        int16x8_t va = vld1q_s16(a + i * 8);
        int16x8_t vb = vld1q_s16(b + i * 8);
        
        // Perform vector multiplication
        int16x8_t vc = vmulq_s16(va, vb);
        
        // Store the result
        vst1q_s16(c + i * 8, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 8; i < len; i++) {
        c[i] = a[i] * b[i];
    }
}

/* 
 * Vector Dot Product Functions
 */

float simd_dot_product_f32(const float* a, const float* b, size_t len) {
    float32x4_t sum_vec = vdupq_n_f32(0.0f);
    
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Multiply and accumulate
        // sum_vec += va * vb
        sum_vec = vmlaq_f32(sum_vec, va, vb);
    }
    
    // Horizontal add to get the final sum
    float32x2_t sum_2 = vadd_f32(vget_low_f32(sum_vec), vget_high_f32(sum_vec));
    sum_2 = vpadd_f32(sum_2, sum_2);
    float sum = vget_lane_f32(sum_2, 0);
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        sum += a[i] * b[i];
    }
    
    return sum;
}

int32_t simd_dot_product_s32(const int32_t* a, const int32_t* b, size_t len) {
    int32x4_t sum_vec = vdupq_n_s32(0);
    
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 int32 elements from each array
        int32x4_t va = vld1q_s32(a + i * 4);
        int32x4_t vb = vld1q_s32(b + i * 4);
        
        // Multiply and accumulate
        sum_vec = vmlaq_s32(sum_vec, va, vb);
    }
    
    // Horizontal add to get the final sum
    int32x2_t sum_2 = vadd_s32(vget_low_s32(sum_vec), vget_high_s32(sum_vec));
    sum_2 = vpadd_s32(sum_2, sum_2);
    int32_t sum = vget_lane_s32(sum_2, 0);
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        sum += a[i] * b[i];
    }
    
    return sum;
}

/* 
 * Vector Comparison Operations
 */

void simd_cmpgt_f32(const float* a, const float* b, uint32_t* result, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector comparison (a > b)
        uint32x4_t vcmp = vcgtq_f32(va, vb);
        
        // Store the result
        vst1q_u32(result + i * 4, vcmp);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        result[i] = (a[i] > b[i]) ? 0xFFFFFFFF : 0;
    }
}

void simd_cmpeq_f32(const float* a, const float* b, uint32_t* result, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector comparison (a == b)
        uint32x4_t vcmp = vceqq_f32(va, vb);
        
        // Store the result
        vst1q_u32(result + i * 4, vcmp);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        result[i] = (a[i] == b[i]) ? 0xFFFFFFFF : 0;
    }
}

/* 
 * Vector Maximum/Minimum Functions
 */

void simd_max_f32(const float* a, const float* b, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector maximum
        float32x4_t vc = vmaxq_f32(va, vb);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = (a[i] > b[i]) ? a[i] : b[i];
    }
}

void simd_min_f32(const float* a, const float* b, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements from each array
        float32x4_t va = vld1q_f32(a + i * 4);
        float32x4_t vb = vld1q_f32(b + i * 4);
        
        // Perform vector minimum
        float32x4_t vc = vminq_f32(va, vb);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = (a[i] < b[i]) ? a[i] : b[i];
    }
}

/* 
 * Other Vector Operations
 */

void simd_abs_f32(const float* a, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements
        float32x4_t va = vld1q_f32(a + i * 4);
        
        // Perform vector absolute value
        float32x4_t vc = vabsq_f32(va);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = (a[i] < 0) ? -a[i] : a[i];
    }
}

void simd_sqrt_f32(const float* a, float* c, size_t len) {
    // Process 4 elements at a time using NEON
    size_t vec_size = len / 4;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 4 float elements
        float32x4_t va = vld1q_f32(a + i * 4);
        
        // Calculate reciprocal square root estimate
        float32x4_t vrsqrte = vrsqrteq_f32(va);
        
        // One Newton-Raphson iteration to improve accuracy
        // rsqrt = rsqrt * (3 - val * rsqrt^2) / 2
        float32x4_t vrsqrte_x2 = vmulq_f32(vrsqrte, vrsqrte);
        float32x4_t vrsqrte_x2_times_a = vmulq_f32(va, vrsqrte_x2);
        float32x4_t vthree_minus = vsubq_f32(vdupq_n_f32(3.0f), vrsqrte_x2_times_a);
        float32x4_t vbetter_rsqrte = vmulq_f32(vrsqrte, vmulq_f32(vthree_minus, vdupq_n_f32(0.5f)));
        
        // Calculate sqrt using rsqrt: sqrt(x) = x * rsqrt(x)
        float32x4_t vc = vmulq_f32(va, vbetter_rsqrte);
        
        // Store the result
        vst1q_f32(c + i * 4, vc);
    }
    
    // Handle remaining elements using standard library
    for (size_t i = vec_size * 4; i < len; i++) {
        c[i] = sqrtf(a[i]);
    }
}

void simd_interleave_even_f32(const float* a, const float* b, float* c, size_t len) {
    // Handle pairs of even elements (0, 2, 4, ...)
    size_t vec_pairs = len / 4;  // Process 2 pairs at a time
    
    for (size_t i = 0; i < vec_pairs; i++) {
        // Load elements from a and b
        float32x4_t va = vld1q_f32(a + i * 4);  // Load a[4i:4i+3]
        float32x4_t vb = vld1q_f32(b + i * 4);  // Load b[4i:4i+3]
        
        // Extract even elements (0, 2)
        float32x2_t va_even = vget_low_f32(va);  // a[4i], a[4i+1]
        float32x2_t vb_even = vget_low_f32(vb);  // b[4i], b[4i+1]
        
        // Interleave first elements
        float32x2_t va_0 = vdup_lane_f32(va_even, 0);  // a[4i], a[4i]
        float32x2_t vb_0 = vdup_lane_f32(vb_even, 0);  // b[4i], b[4i]
        float32x2_t vinterleave_0 = vzip1_f32(va_0, vb_0);  // a[4i], b[4i]
        
        // Interleave third elements
        float32x2_t va_2 = vdup_lane_f32(va_even, 1);  // a[4i+1], a[4i+1]
        float32x2_t vb_2 = vdup_lane_f32(vb_even, 1);  // b[4i+1], b[4i+1]
        float32x2_t vinterleave_2 = vzip1_f32(va_2, vb_2);  // a[4i+1], b[4i+1]
        
        // Store the results
        vst1_f32(c + i * 4, vinterleave_0);
        vst1_f32(c + i * 4 + 2, vinterleave_2);
    }
    
    // Handle remaining elements (if len is not a multiple of 4)
    size_t remaining_start = vec_pairs * 4;
    for (size_t i = remaining_start / 2; i < len / 2; i++) {
        c[i * 2] = a[i];
        c[i * 2 + 1] = b[i];
    }
}

void simd_interleave_odd_f32(const float* a, const float* b, float* c, size_t len) {
    // Handle pairs of odd elements (1, 3, 5, ...)
    size_t vec_pairs = len / 4;  // Process 2 pairs at a time
    
    for (size_t i = 0; i < vec_pairs; i++) {
        // Load elements from a and b
        float32x4_t va = vld1q_f32(a + i * 4);  // Load a[4i:4i+3]
        float32x4_t vb = vld1q_f32(b + i * 4);  // Load b[4i:4i+3]
        
        // Extract odd elements (1, 3)
        float32x2_t va_odd = vget_high_f32(va);  // a[4i+2], a[4i+3]
        float32x2_t vb_odd = vget_high_f32(vb);  // b[4i+2], b[4i+3]
        
        // Interleave second elements
        float32x2_t va_1 = vdup_lane_f32(va_odd, 0);  // a[4i+2], a[4i+2]
        float32x2_t vb_1 = vdup_lane_f32(vb_odd, 0);  // b[4i+2], b[4i+2]
        float32x2_t vinterleave_1 = vzip1_f32(va_1, vb_1);  // a[4i+2], b[4i+2]
        
        // Interleave fourth elements
        float32x2_t va_3 = vdup_lane_f32(va_odd, 1);  // a[4i+3], a[4i+3]
        float32x2_t vb_3 = vdup_lane_f32(vb_odd, 1);  // b[4i+3], b[4i+3]
        float32x2_t vinterleave_3 = vzip1_f32(va_3, vb_3);  // a[4i+3], b[4i+3]
        
        // Store the results
        vst1_f32(c + i * 4, vinterleave_1);
        vst1_f32(c + i * 4 + 2, vinterleave_3);
    }
    
    // Handle remaining elements (if len is not a multiple of 4)
    size_t remaining_start = vec_pairs * 4;
    for (size_t i = remaining_start / 2; i < len / 2; i++) {
        if (2 * i + 1 < len) {
            c[i * 2] = a[2 * i + 1];
            c[i * 2 + 1] = b[2 * i + 1];
        }
    }
}

/* 
 * Image Processing Operations
 */

void simd_rgb_to_gray(const uint8_t* rgb, uint8_t* gray, size_t pixel_count) {
    // RGB coefficients for grayscale conversion: R:0.299, G:0.587, B:0.114
    // Scale by 256 for fixed-point calculation
    const uint8_t r_coeff = 77;  // 0.299 * 256 ≈ 77
    const uint8_t g_coeff = 150; // 0.587 * 256 ≈ 150
    const uint8_t b_coeff = 29;  // 0.114 * 256 ≈ 29
    
    // Create vectors with coefficients
    uint8x8_t vr_coeff = vdup_n_u8(r_coeff);
    uint8x8_t vg_coeff = vdup_n_u8(g_coeff);
    uint8x8_t vb_coeff = vdup_n_u8(b_coeff);
    
    // Process 8 pixels at a time
    size_t vec_size = pixel_count / 8;
    
    for (size_t i = 0; i < vec_size; i++) {
        // Load 8 RGB pixels (24 bytes)
        const uint8_t* pixel_ptr = rgb + i * 24;
        
        // Extract R, G, B channels
        uint8x8x3_t rgb_pixels = vld3_u8(pixel_ptr);
        uint8x8_t r_channel = rgb_pixels.val[0];
        uint8x8_t g_channel = rgb_pixels.val[1];
        uint8x8_t b_channel = rgb_pixels.val[2];
        
        // Multiply each channel by its coefficient
        uint16x8_t r_contrib = vmull_u8(r_channel, vr_coeff);
        uint16x8_t g_contrib = vmull_u8(g_channel, vg_coeff);
        uint16x8_t b_contrib = vmull_u8(b_channel, vb_coeff);
        
        // Sum the contributions
        uint16x8_t sum = vaddq_u16(r_contrib, g_contrib);
        sum = vaddq_u16(sum, b_contrib);
        
        // Divide by 256 (shift right by 8)
        uint8x8_t gray_pixels = vshrn_n_u16(sum, 8);
        
        // Store the result
        vst1_u8(gray + i * 8, gray_pixels);
    }
    
    // Handle remaining pixels
    for (size_t i = vec_size * 8; i < pixel_count; i++) {
        const uint8_t* pixel = rgb + i * 3;
        uint16_t gray_val = (pixel[0] * r_coeff + pixel[1] * g_coeff + pixel[2] * b_coeff) >> 8;
        gray[i] = (uint8_t)gray_val;
    }
}

void simd_blur_gray_3x3(const uint8_t* input, uint8_t* output, int width, int height) {
    // 3x3 Box blur (simple average filter)
    // Skip the border pixels for simplicity
    
    // Create a vector of 9 (for the 3x3 kernel divisor)
    uint8x8_t vdiv9 = vdup_n_u8(9);
    
    for (int y = 1; y < height - 1; y++) {
        // Process 8 pixels at a time
        for (int x = 1; x < width - 8; x += 8) {
            // Initialize accumulator
            uint16x8_t sum = vdupq_n_u16(0);
            
            // Accumulate 3x3 neighborhood for 8 adjacent pixels
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8x8_t neighborhood = vld1_u8(input + (y + ky) * width + (x + kx));
                    sum = vaddw_u8(sum, neighborhood);
                }
            }
            
            // Divide by 9 (approximate using multiply and shift)
            // Using fixed-point: multiply by 255/9 ≈ 28.33.. and then shift right by 8
            uint16x8_t scaled = vmulq_n_u16(sum, 28);
            uint8x8_t result = vshrn_n_u16(scaled, 8);
            
            // Store the result
            vst1_u8(output + y * width + x, result);
        }
        
        // Handle remaining pixels
        for (int x = width - (width - 1) % 8; x < width - 1; x++) {
            uint16_t sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += input[(y + ky) * width + (x + kx)];
                }
            }
            output[y * width + x] = (uint8_t)(sum / 9);
        }
    }
    
    // Handle border pixels (just copy from input)
    for (int x = 0; x < width; x++) {
        output[x] = input[x];  // Top row
        output[(height - 1) * width + x] = input[(height - 1) * width + x];  // Bottom row
    }
    
    for (int y = 1; y < height - 1; y++) {
        output[y * width] = input[y * width];  // Left column
        output[y * width + width - 1] = input[y * width + width - 1];  // Right column
    }
}
