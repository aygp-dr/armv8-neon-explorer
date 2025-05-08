/**
 * box_blur.c
 * Demonstrates 3x3 box blur using ARM NEON SIMD
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
void scalar_blur_gray_3x3(const uint8_t* input, uint8_t* output, int width, int height) {
    // Skip the border pixels for simplicity
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            uint32_t sum = 0;
            
            // 3x3 Box blur (simple average filter)
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += input[(y + ky) * width + (x + kx)];
                }
            }
            
            // Divide by 9 (number of pixels in 3x3 kernel)
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

// NEON SIMD implementation
void neon_blur_gray_3x3(const uint8_t* input, uint8_t* output, int width, int height) {
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

// Generate a synthetic grayscale test image
void generate_test_image(uint8_t* gray, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a pattern with some detail (checkerboard + gradient)
            uint8_t checker = ((x / 8 + y / 8) % 2) * 128;
            uint8_t gradient = (uint8_t)((x * y) % 128);
            gray[y * width + x] = checker + gradient;
        }
    }
}

// Function to save a grayscale image as PGM (for visualization)
void save_pgm(const char* filename, const uint8_t* gray, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("ERROR: Could not open file %s for writing.\n", filename);
        return;
    }
    
    // Write PGM header
    fprintf(file, "P5\n%d %d\n255\n", width, height);
    
    // Write image data
    fwrite(gray, 1, width * height, file);
    
    fclose(file);
}

// Main function
int main(int argc, char** argv) {
    // Default image size
    int width = 512;
    int height = 512;
    
    // Allow overriding image size from command line
    if (argc > 2) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        if (width <= 0 || height <= 0) {
            width = 512;
            height = 512;
        }
    }
    
    printf("Box Blur (3x3) Example\n");
    printf("---------------------\n");
    printf("Image size: %dx%d\n", width, height);
    
    // Check if NEON is supported
    if (!check_neon_support()) {
        printf("ERROR: ARM NEON is not supported on this platform.\n");
        return 1;
    }
    
    // Print platform information
    print_platform_info();
    
    // Allocate memory for images
    uint8_t* input = (uint8_t*)malloc(width * height);
    uint8_t* output_neon = (uint8_t*)malloc(width * height);
    uint8_t* output_scalar = (uint8_t*)malloc(width * height);
    
    if (!input || !output_neon || !output_scalar) {
        printf("ERROR: Memory allocation failed.\n");
        return 1;
    }
    
    // Generate a test image
    generate_test_image(input, width, height);
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Box Blur (3x3)");
    
    // Number of iterations for more accurate timing
    const int iterations = 20;
    
    // Perform blur using NEON
    timer_start(comp->simd_timer);
    for (int i = 0; i < iterations; i++) {
        neon_blur_gray_3x3(input, output_neon, width, height);
    }
    timer_stop(comp->simd_timer);
    
    // Perform blur using scalar code
    timer_start(comp->scalar_timer);
    for (int i = 0; i < iterations; i++) {
        scalar_blur_gray_3x3(input, output_scalar, width, height);
    }
    timer_stop(comp->scalar_timer);
    
    // Verify results
    int errors = 0;
    int max_diff = 0;
    
    for (int i = 0; i < width * height; i++) {
        int diff = abs((int)output_neon[i] - (int)output_scalar[i]);
        if (diff > 1) {  // Allow small differences due to fixed-point approximation
            errors++;
            if (diff > max_diff) {
                max_diff = diff;
            }
        }
    }
    
    printf("Verification: %d errors (max diff: %d) out of %d pixels\n", 
           errors, max_diff, width * height);
    
    // Save images for visual inspection
    save_pgm("input.pgm", input, width, height);
    save_pgm("output_neon.pgm", output_neon, width, height);
    save_pgm("output_scalar.pgm", output_scalar, width, height);
    
    printf("Saved images: input.pgm, output_neon.pgm, output_scalar.pgm\n");
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(input);
    free(output_neon);
    free(output_scalar);
    comparison_destroy(comp);
    
    return 0;
}
