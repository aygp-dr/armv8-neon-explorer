/**
 * vector_add_example.c
 * Demonstrates vector addition using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "../include/simd_ops.h"
#include "../include/perf_test.h"

// Scalar (non-SIMD) implementation for comparison
void scalar_add_f32(const float* a, const float* b, float* c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c[i] = a[i] + b[i];
    }
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
    
    printf("Vector Addition Example\n");
    printf("----------------------\n");
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
    float* c_simd = (float*)neon_malloc(vector_size * sizeof(float));
    float* c_scalar = (float*)neon_malloc(vector_size * sizeof(float));
    
    if (!a || !b || !c_simd || !c_scalar) {
        printf("ERROR: Memory allocation failed.\n");
        return 1;
    }
    
    // Initialize vectors with random data
    srand(time(NULL));
    fill_random_float(a, vector_size, -100.0f, 100.0f);
    fill_random_float(b, vector_size, -100.0f, 100.0f);
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Vector Addition (Float)");
    
    // Perform vector addition using SIMD
    timer_start(comp->simd_timer);
    simd_add_f32(a, b, c_simd, vector_size);
    timer_stop(comp->simd_timer);
    
    // Perform vector addition using scalar code
    timer_start(comp->scalar_timer);
    scalar_add_f32(a, b, c_scalar, vector_size);
    timer_stop(comp->scalar_timer);
    
    // Verify results
    float max_diff = 0.0f;
    size_t errors = 0;
    
    for (size_t i = 0; i < vector_size; i++) {
        float diff = fabsf(c_simd[i] - c_scalar[i]);
        if (diff > max_diff) {
            max_diff = diff;
        }
        if (diff > 1e-5) {
            errors++;
        }
    }
    
    printf("Verification: %zu errors, max difference: %.10f\n", 
           errors, max_diff);
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(a);
    free(b);
    free(c_simd);
    free(c_scalar);
    comparison_destroy(comp);
    
    return 0;
}
