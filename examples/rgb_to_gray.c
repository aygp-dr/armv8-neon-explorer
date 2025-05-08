/**
 * rgb_to_gray.c
 * Demonstrates RGB to grayscale conversion using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <arm_neon.h>
#include "../include/platform_detect.h"
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Scalar (non-SIMD) implementation for comparison
void scalar_rgb_to_gray(const uint8_t* rgb, uint8_t* gray, size_t pixel_count) {
    for (size_t i = 0; i < pixel_count; i++) {
        const uint8_t* pixel = rgb + i * 3;
        // Standard grayscale conversion: Y = 0.299*R + 0.587*G + 0.114*B
        uint32_t r = pixel[0];
        uint32_t g = pixel[1];
        uint32_t b = pixel[2];
        gray[i] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);  // Fixed-point calculation
    }
}

// NEON SIMD implementation
void neon_rgb_to_gray(const uint8_t* rgb, uint8_t* gray, size_t pixel_count) {
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

// Generate a synthetic RGB image
void generate_test_image(uint8_t* rgb, size_t pixel_count) {
    for (size_t i = 0; i < pixel_count; i++) {
        // Create a gradient pattern
        uint8_t r = (uint8_t)(i % 256);
        uint8_t g = (uint8_t)((i / 256) % 256);
        uint8_t b = (uint8_t)((i / 65536) % 256);
        
        rgb[i * 3 + 0] = r;
        rgb[i * 3 + 1] = g;
        rgb[i * 3 + 2] = b;
    }
}

// Main function
int main(int argc, char** argv) {
    // Default image size
    size_t width = 1024;
    size_t height = 768;
    
    // Allow overriding image size from command line
    if (argc > 2) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        if (width <= 0 || height <= 0) {
            width = 1024;
            height = 768;
        }
    }
    
    size_t pixel_count = width * height;
    
    printf("RGB to Grayscale Conversion Example\n");
    printf("----------------------------------\n");
    printf("Image size: %zux%zu (%zu pixels)\n", width, height, pixel_count);
    
    // Check if NEON is supported
    if (!check_neon_support()) {
        printf("ERROR: ARM NEON is not supported on this platform.\n");
        return 1;
    }
    
    // Print platform information
    print_platform_info();
    
    // Allocate memory for images
    uint8_t* rgb = (uint8_t*)malloc(pixel_count * 3);  // RGB (3 bytes per pixel)
    uint8_t* gray_neon = (uint8_t*)malloc(pixel_count);   // Grayscale (1 byte per pixel)
    uint8_t* gray_scalar = (uint8_t*)malloc(pixel_count); // Grayscale (1 byte per pixel)
    
    if (!rgb || !gray_neon || !gray_scalar) {
        printf("ERROR: Memory allocation failed.\n");
        return 1;
    }
    
    // Generate a test image
    generate_test_image(rgb, pixel_count);
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("RGB to Grayscale");
    
    // Number of iterations for more accurate timing
    const int iterations = 10;
    
    // Perform RGB to grayscale conversion using NEON
    timer_start(comp->simd_timer);
    for (int i = 0; i < iterations; i++) {
        neon_rgb_to_gray(rgb, gray_neon, pixel_count);
    }
    timer_stop(comp->simd_timer);
    
    // Perform RGB to grayscale conversion using scalar code
    timer_start(comp->scalar_timer);
    for (int i = 0; i < iterations; i++) {
        scalar_rgb_to_gray(rgb, gray_scalar, pixel_count);
    }
    timer_stop(comp->scalar_timer);
    
    // Verify results
    int errors = 0;
    for (size_t i = 0; i < pixel_count; i++) {
        if (gray_neon[i] != gray_scalar[i]) {
            errors++;
        }
    }
    
    printf("Verification: %d errors out of %zu pixels\n", errors, pixel_count);
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(rgb);
    free(gray_neon);
    free(gray_scalar);
    comparison_destroy(comp);
    
    return 0;
}
