/**
 * sobel_edge.c
 * Demonstrates Sobel edge detection using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <arm_neon.h>
#include "../include/platform_detect.h"
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Scalar (non-SIMD) implementation for comparison
void scalar_sobel_edge(const uint8_t* input, uint8_t* output, int width, int height) {
    // Sobel kernels for x and y gradients
    const int sobel_x[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    const int sobel_y[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
    
    // Skip the border pixels for simplicity
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0;
            int gy = 0;
            
            // Apply Sobel kernels
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int pixel = input[(y + ky) * width + (x + kx)];
                    gx += pixel * sobel_x[ky + 1][kx + 1];
                    gy += pixel * sobel_y[ky + 1][kx + 1];
                }
            }
            
            // Compute magnitude
            int magnitude = sqrt(gx * gx + gy * gy);
            
            // Clamp to [0, 255]
            if (magnitude > 255) magnitude = 255;
            
            output[y * width + x] = (uint8_t)magnitude;
        }
    }
    
    // Handle border pixels (set to 0)
    for (int x = 0; x < width; x++) {
        output[x] = 0;  // Top row
        output[(height - 1) * width + x] = 0;  // Bottom row
    }
    
    for (int y = 1; y < height - 1; y++) {
        output[y * width] = 0;  // Left column
        output[y * width + width - 1] = 0;  // Right column
    }
}

// NEON SIMD implementation
void neon_sobel_edge(const uint8_t* input, uint8_t* output, int width, int height) {
    // Skip the border pixels for simplicity
    for (int y = 1; y < height - 1; y++) {
        // Process 8 pixels at a time
        for (int x = 1; x < width - 9; x += 8) {
            // Load 3x3 neighborhood for 8 adjacent pixels
            // Load 10 pixels for each row (8 + 2 border pixels)
            uint8x16_t top_row = vld1q_u8(input + (y - 1) * width + (x - 1));
            uint8x16_t mid_row = vld1q_u8(input + y * width + (x - 1));
            uint8x16_t bot_row = vld1q_u8(input + (y + 1) * width + (x - 1));
            
            // Extract relevant parts for our 3x3 window
            // For first 8 pixels we need indices 0-9 from each row
            uint8x8_t top_left = vget_low_u8(top_row);       // top row [0-7]
            uint8x8_t top_right = vext_u8(vget_low_u8(top_row), vget_high_u8(top_row), 1); // top row [1-8]
            uint8x8_t top_more_right = vext_u8(vget_low_u8(top_row), vget_high_u8(top_row), 2); // top row [2-9]
            
            uint8x8_t mid_left = vget_low_u8(mid_row);       // mid row [0-7]
            uint8x8_t mid_right = vext_u8(vget_low_u8(mid_row), vget_high_u8(mid_row), 1); // mid row [1-8]
            uint8x8_t mid_more_right = vext_u8(vget_low_u8(mid_row), vget_high_u8(mid_row), 2); // mid row [2-9]
            
            uint8x8_t bot_left = vget_low_u8(bot_row);       // bottom row [0-7]
            uint8x8_t bot_right = vext_u8(vget_low_u8(bot_row), vget_high_u8(bot_row), 1); // bottom row [1-8]
            uint8x8_t bot_more_right = vext_u8(vget_low_u8(bot_row), vget_high_u8(bot_row), 2); // bottom row [2-9]
            
            // Convert from uint8x8_t to int16x8_t for calculations
            int16x8_t top_left_16 = vreinterpretq_s16_u16(vmovl_u8(top_left));
            int16x8_t top_right_16 = vreinterpretq_s16_u16(vmovl_u8(top_right));
            int16x8_t top_more_right_16 = vreinterpretq_s16_u16(vmovl_u8(top_more_right));
            
            int16x8_t mid_left_16 = vreinterpretq_s16_u16(vmovl_u8(mid_left));
            int16x8_t mid_more_right_16 = vreinterpretq_s16_u16(vmovl_u8(mid_more_right));
            
            int16x8_t bot_left_16 = vreinterpretq_s16_u16(vmovl_u8(bot_left));
            int16x8_t bot_right_16 = vreinterpretq_s16_u16(vmovl_u8(bot_right));
            int16x8_t bot_more_right_16 = vreinterpretq_s16_u16(vmovl_u8(bot_more_right));
            
            // Calculate Gx
            // Gx = -1*top_left + 1*top_more_right + -2*mid_left + 2*mid_more_right + -1*bot_left + 1*bot_more_right
            int16x8_t gx = vnegq_s16(top_left_16);                 // -1 * top_left
            gx = vaddq_s16(gx, top_more_right_16);                // + 1 * top_more_right
            gx = vsubq_s16(gx, vshlq_n_s16(mid_left_16, 1));      // - 2 * mid_left
            gx = vaddq_s16(gx, vshlq_n_s16(mid_more_right_16, 1)); // + 2 * mid_more_right
            gx = vsubq_s16(gx, bot_left_16);                      // - 1 * bot_left
            gx = vaddq_s16(gx, bot_more_right_16);                // + 1 * bot_more_right
            
            // Calculate Gy
            // Gy = -1*top_left + -2*top_right + -1*top_more_right + 1*bot_left + 2*bot_right + 1*bot_more_right
            int16x8_t gy = vnegq_s16(top_left_16);                 // -1 * top_left
            gy = vsubq_s16(gy, vshlq_n_s16(top_right_16, 1));     // - 2 * top_right
            gy = vsubq_s16(gy, top_more_right_16);                // - 1 * top_more_right
            gy = vaddq_s16(gy, bot_left_16);                      // + 1 * bot_left
            gy = vaddq_s16(gy, vshlq_n_s16(bot_right_16, 1));     // + 2 * bot_right
            gy = vaddq_s16(gy, bot_more_right_16);                // + 1 * bot_more_right
            
            // Calculate magnitude approximation: |Gx| + |Gy| (faster than sqrt(Gx^2 + Gy^2))
            int16x8_t abs_gx = vabsq_s16(gx);
            int16x8_t abs_gy = vabsq_s16(gy);
            int16x8_t sum = vaddq_s16(abs_gx, abs_gy);
            
            // Scale down (approximation)
            sum = vshrq_n_s16(sum, 1);
            
            // Clamp to [0, 255]
            uint8x8_t result = vqmovun_s16(sum);
            
            // Store the result
            vst1_u8(output + y * width + x, result);
        }
        
        // Handle remaining pixels
        for (int x = width - (width - 1) % 8; x < width - 1; x++) {
            int gx = 0;
            int gy = 0;
            
            // Apply Sobel kernels
            gx -= input[(y - 1) * width + (x - 1)];     // top left
            gx += input[(y - 1) * width + (x + 1)];     // top right
            gx -= 2 * input[y * width + (x - 1)];       // middle left
            gx += 2 * input[y * width + (x + 1)];       // middle right
            gx -= input[(y + 1) * width + (x - 1)];     // bottom left
            gx += input[(y + 1) * width + (x + 1)];     // bottom right
            
            gy -= input[(y - 1) * width + (x - 1)];     // top left
            gy -= 2 * input[(y - 1) * width + x];       // top middle
            gy -= input[(y - 1) * width + (x + 1)];     // top right
            gy += input[(y + 1) * width + (x - 1)];     // bottom left
            gy += 2 * input[(y + 1) * width + x];       // bottom middle
            gy += input[(y + 1) * width + (x + 1)];     // bottom right
            
            // Compute magnitude (approximation)
            int magnitude = (abs(gx) + abs(gy)) >> 1;
            
            // Clamp to [0, 255]
            if (magnitude > 255) magnitude = 255;
            
            output[y * width + x] = (uint8_t)magnitude;
        }
    }
    
    // Handle border pixels (set to 0)
    for (int x = 0; x < width; x++) {
        output[x] = 0;  // Top row
        output[(height - 1) * width + x] = 0;  // Bottom row
    }
    
    for (int y = 1; y < height - 1; y++) {
        output[y * width] = 0;  // Left column
        output[y * width + width - 1] = 0;  // Right column
    }
}

// Generate a synthetic grayscale test image
void generate_test_image(uint8_t* gray, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a pattern with some edges (circles)
            int cx = width / 2;
            int cy = height / 2;
            int dx = x - cx;
            int dy = y - cy;
            int dist = sqrt(dx * dx + dy * dy);
            
            // Multiple concentric circles
            uint8_t val = 0;
            if (dist % 32 < 16) {
                val = 255;
            }
            
            // Add some noise
            int noise = rand() % 25;
            val = (val + noise > 255) ? 255 : val + noise;
            
            gray[y * width + x] = val;
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
    
    // Seed random number generator
    srand(42);  // Fixed seed for reproducibility
    
    printf("Sobel Edge Detection Example\n");
    printf("---------------------------\n");
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
    perf_comparison_t* comp = comparison_create("Sobel Edge Detection");
    
    // Number of iterations for more accurate timing
    const int iterations = 20;
    
    // Perform edge detection using NEON
    timer_start(comp->simd_timer);
    for (int i = 0; i < iterations; i++) {
        neon_sobel_edge(input, output_neon, width, height);
    }
    timer_stop(comp->simd_timer);
    
    // Perform edge detection using scalar code
    timer_start(comp->scalar_timer);
    for (int i = 0; i < iterations; i++) {
        scalar_sobel_edge(input, output_scalar, width, height);
    }
    timer_stop(comp->scalar_timer);
    
    // Verify results
    int errors = 0;
    int max_diff = 0;
    
    for (int i = 0; i < width * height; i++) {
        int diff = abs((int)output_neon[i] - (int)output_scalar[i]);
        if (diff > 5) {  // Allow small differences due to approximation
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
    save_pgm("edges_neon.pgm", output_neon, width, height);
    save_pgm("edges_scalar.pgm", output_scalar, width, height);
    
    printf("Saved images: input.pgm, edges_neon.pgm, edges_scalar.pgm\n");
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(input);
    free(output_neon);
    free(output_scalar);
    comparison_destroy(comp);
    
    return 0;
}
