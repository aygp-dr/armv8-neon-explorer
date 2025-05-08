/**
 * dot_product_example.c
 * Demonstrates vector dot product computation using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "../include/simd_ops.h"
#include "../include/perf_test.h"

// Scalar (non-SIMD) implementation for comparison
float scalar_dot_product_f32(const float* a, const float* b, size_t len) {
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

int main(int argc, char** argv) {
    // Default vector size
    size_t vector_size = 1024 * 1024;  // 1M elements
    
    // Allow overriding vector size from command line
    if (argc > 1) {
        vector_size = atoi(argv[1]);
        if (vector_size <= 0) {
            vector_size = 1024 * 1024;
        }
    }
    
    printf("Dot Product Example\n");
    printf("------------------\n");
    printf("Vector size: %zu elements\n", vector_size);
    
    // Check if NEON is supported
    if (!check_neon_support()) {
        printf("ERROR: ARM NEON is not supported on this platform.\n");
        return 1;
    }
    
    // Print platform information
    print_platform_info();
    
    // Allocate aligned memory for vectors
    float* a = (float*)neon_malloc(vector_size * sizeof(float));
    float* b = (float*)neon_malloc(vector_size * sizeof(float));
    
    if (!a || !b) {
        printf("ERROR: Memory allocation failed.\n");
        return 1;
    }
    
    // Initialize vectors with random data
    srand(time(NULL));
    fill_random_float(a, vector_size, -10.0f, 10.0f);
    fill_random_float(b, vector_size, -10.0f, 10.0f);
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Dot Product (Float)");
    
    // Warm up the cache
    volatile float dummy_simd = simd_dot_product_f32(a, b, vector_size);
    volatile float dummy_scalar = scalar_dot_product_f32(a, b, vector_size);
    (void)dummy_simd;
    (void)dummy_scalar;
    
    // Number of iterations for more accurate timing
    const int iterations = 20;
    float simd_result = 0.0f;
    float scalar_result = 0.0f;
    
    // Perform dot product using SIMD
    timer_start(comp->simd_timer);
    for (int i = 0; i < iterations; i++) {
        simd_result = simd_dot_product_f32(a, b, vector_size);
    }
    timer_stop(comp->simd_timer);
    
    // Perform dot product using scalar code
    timer_start(comp->scalar_timer);
    for (int i = 0; i < iterations; i++) {
        scalar_result = scalar_dot_product_f32(a, b, vector_size);
    }
    timer_stop(comp->scalar_timer);
    
    // Verify results
    float diff = fabsf(simd_result - scalar_result);
    float rel_diff = diff / fabsf(scalar_result);
    
    printf("SIMD Result: %.10f\n", simd_result);
    printf("Scalar Result: %.10f\n", scalar_result);
    printf("Absolute Difference: %.10f\n", diff);
    printf("Relative Difference: %.10f%%\n", rel_diff * 100.0f);
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(a);
    free(b);
    comparison_destroy(comp);
    
    return 0;
}
