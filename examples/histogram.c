/**
 * histogram.c
 * Demonstrates histogram calculation using ARM NEON SIMD
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
void scalar_histogram(const uint8_t* image, uint32_t* histogram, int size) {
    // Clear histogram
    memset(histogram, 0, 256 * sizeof(uint32_t));
    
    // Count occurrences
    for (int i = 0; i < size; i++) {
        histogram[image[i]]++;
    }
}

// NEON SIMD implementation
void neon_histogram(const uint8_t* image, uint32_t* histogram, int size) {
    // Clear histogram
    memset(histogram, 0, 256 * sizeof(uint32_t));
    
    // Process data in chunks of 16 bytes
    int chunks = size / 16;
    
    // Use local histograms to avoid memory conflicts
    uint16_t local_hist[4][256];
    memset(local_hist, 0, 4 * 256 * sizeof(uint16_t));
    
    for (int i = 0; i < chunks; i++) {
        // Load 16 pixels
        uint8x16_t pixels = vld1q_u8(image + i * 16);
        
        // Extract bytes to process them separately
        uint8x8_t low = vget_low_u8(pixels);
        uint8x8_t high = vget_high_u8(pixels);
        
        // Further split to avoid conflicts when incrementing
        uint8x4_t low1 = vget_low_u8(low);
        uint8x4_t low2 = vget_high_u8(low);
        uint8x4_t high1 = vget_low_u8(high);
        uint8x4_t high2 = vget_high_u8(high);
        
        // Extract individual values (we need to do this because NEON doesn't have 
        // scatter operations to increment arbitrary memory locations)
        uint8_t values[16];
        vst1_u8(values, low);
        vst1_u8(values + 8, high);
        
        // Update histograms (still need to do this in scalar fashion)
        // Using separate local histograms to avoid conflicts
        for (int j = 0; j < 4; j++) {
            local_hist[0][values[j]]++;
            local_hist[1][values[j + 4]]++;
            local_hist[2][values[j + 8]]++;
            local_hist[3][values[j + 12]]++;
        }
    }
    
    // Process remaining pixels
    for (int i = chunks * 16; i < size; i++) {
        local_hist[0][image[i]]++;
    }
    
    // Combine local histograms into the final result
    for (int i = 0; i < 256; i++) {
        histogram[i] = local_hist[0][i] + local_hist[1][i] + local_hist[2][i] + local_hist[3][i];
    }
}

// Generate a test image
void generate_test_image(uint8_t* image, int size) {
    for (int i = 0; i < size; i++) {
        image[i] = rand() % 256;
    }
}

// Main function
int main(int argc, char** argv) {
    // Default image size
    int width = 1024;
    int height = 768;
    
    // Allow overriding image size from command line
    if (argc > 2) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        if (width <= 0 || height <= 0) {
            width = 1024;
            height = 768;
        }
    }
    
    int size = width * height;
    
    printf("Histogram Calculation Example\n");
    printf("----------------------------\n");
    printf("Image size: %dx%d (%d pixels)\n", width, height, size);
    
    // Check if NEON is supported
    if (!check_neon_support()) {
        printf("ERROR: ARM NEON is not supported on this platform.\n");
        return 1;
    }
    
    // Print platform information
    print_platform_info();
    
    // Allocate memory
    uint8_t* image = (uint8_t*)malloc(size);
    uint32_t* hist_neon = (uint32_t*)malloc(256 * sizeof(uint32_t));
    uint32_t* hist_scalar = (uint32_t*)malloc(256 * sizeof(uint32_t));
    
    if (!image || !hist_neon || !hist_scalar) {
        printf("ERROR: Memory allocation failed.\n");
        return 1;
    }
    
    // Generate test image
    srand(time(NULL));
    generate_test_image(image, size);
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Histogram Calculation");
    
    // Number of iterations for more accurate timing
    const int iterations = 20;
    
    // Calculate histogram using NEON
    timer_start(comp->simd_timer);
    for (int i = 0; i < iterations; i++) {
        neon_histogram(image, hist_neon, size);
    }
    timer_stop(comp->simd_timer);
    
    // Calculate histogram using scalar code
    timer_start(comp->scalar_timer);
    for (int i = 0; i < iterations; i++) {
        scalar_histogram(image, hist_scalar, size);
    }
    timer_stop(comp->scalar_timer);
    
    // Verify results
    int errors = 0;
    for (int i = 0; i < 256; i++) {
        if (hist_neon[i] != hist_scalar[i]) {
            errors++;
            printf("Mismatch at bin %d: NEON=%u, Scalar=%u\n", 
                   i, hist_neon[i], hist_scalar[i]);
        }
    }
    
    printf("Verification: %d errors out of 256 bins\n", errors);
    
    // Print some histogram stats
    uint32_t min_value = 0xFFFFFFFF;
    uint32_t max_value = 0;
    float avg_value = 0.0f;
    
    for (int i = 0; i < 256; i++) {
        if (hist_scalar[i] < min_value) min_value = hist_scalar[i];
        if (hist_scalar[i] > max_value) max_value = hist_scalar[i];
        avg_value += hist_scalar[i];
    }
    avg_value /= 256.0f;
    
    printf("Histogram stats: min=%u, max=%u, avg=%.2f\n", 
           min_value, max_value, avg_value);
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(image);
    free(hist_neon);
    free(hist_scalar);
    comparison_destroy(comp);
    
    return 0;
}
