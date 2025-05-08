/**
 * test_basic_ops.c
 * Unit tests for basic vector operations
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/simd_ops.h"
#include "../include/neon_utils.h"
#include "../include/test_framework.h"
#include "../include/perf_test.h"

// Scalar implementation for addition
void scalar_add_f32(const float* a, const float* b, float* c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

// Scalar implementation for multiplication
void scalar_mul_f32(const float* a, const float* b, float* c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c[i] = a[i] * b[i];
    }
}

// Scalar implementation for dot product
float scalar_dot_product_f32(const float* a, const float* b, size_t len) {
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// Test vector addition
void test_vector_addition(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    float* c_simd = (float*)neon_malloc(test_size * sizeof(float));
    float* c_scalar = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b || !c_simd || !c_scalar) {
        test_suite_add_result(suite, "Vector Addition - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    simd_add_f32(a, b, c_simd, test_size);
    scalar_add_f32(a, b, c_scalar, test_size);
    
    // Verify results
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Results", c_simd, c_scalar, test_size, 1e-6f);
    
    // Test edge cases
    
    // 1. Test with unaligned size
    const size_t unaligned_size = test_size - 3;
    memset(c_simd, 0, test_size * sizeof(float));
    memset(c_scalar, 0, test_size * sizeof(float));
    
    simd_add_f32(a, b, c_simd, unaligned_size);
    scalar_add_f32(a, b, c_scalar, unaligned_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Unaligned Size", 
                          c_simd, c_scalar, unaligned_size, 1e-6f);
    
    // 2. Test with small arrays (smaller than SIMD width)
    const size_t small_size = 3;
    simd_add_f32(a, b, c_simd, small_size);
    scalar_add_f32(a, b, c_scalar, small_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Small Arrays", 
                          c_simd, c_scalar, small_size, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
    free(c_simd);
    free(c_scalar);
}

// Test vector multiplication
void test_vector_multiplication(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    float* c_simd = (float*)neon_malloc(test_size * sizeof(float));
    float* c_scalar = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b || !c_simd || !c_scalar) {
        test_suite_add_result(suite, "Vector Multiplication - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    simd_mul_f32(a, b, c_simd, test_size);
    scalar_mul_f32(a, b, c_scalar, test_size);
    
    // Verify results
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Multiplication - Results", 
                          c_simd, c_scalar, test_size, 1e-6f);
    
    // Test edge cases (similar to addition tests)
    const size_t unaligned_size = test_size - 3;
    memset(c_simd, 0, test_size * sizeof(float));
    memset(c_scalar, 0, test_size * sizeof(float));
    
    simd_mul_f32(a, b, c_simd, unaligned_size);
    scalar_mul_f32(a, b, c_scalar, unaligned_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Multiplication - Unaligned Size", 
                          c_simd, c_scalar, unaligned_size, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
    free(c_simd);
    free(c_scalar);
}

// Test dot product
void test_dot_product(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b) {
        test_suite_add_result(suite, "Dot Product - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    float result_simd = simd_dot_product_f32(a, b, test_size);
    float result_scalar = scalar_dot_product_f32(a, b, test_size);
    
    // Verify results (allow small epsilon due to different summation order)
    ASSERT_FLOAT_EQ(suite, "Dot Product - Full Size", result_simd, result_scalar, 1e-4f);
    
    // Test edge cases
    const size_t unaligned_size = test_size - 3;
    float result_simd_unaligned = simd_dot_product_f32(a, b, unaligned_size);
    float result_scalar_unaligned = scalar_dot_product_f32(a, b, unaligned_size);
    
    ASSERT_FLOAT_EQ(suite, "Dot Product - Unaligned Size", 
                    result_simd_unaligned, result_scalar_unaligned, 1e-4f);
    
    // Test with small arrays
    const size_t small_size = 3;
    float result_simd_small = simd_dot_product_f32(a, b, small_size);
    float result_scalar_small = scalar_dot_product_f32(a, b, small_size);
    
    ASSERT_FLOAT_EQ(suite, "Dot Product - Small Arrays", 
                    result_simd_small, result_scalar_small, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
}

// Main test function
int main() {
    printf("Running unit tests for basic vector operations...\n");
    
    // Create test suite
    test_suite_t* suite = test_suite_create("Basic Vector Operations");
    
    // Run tests
    test_vector_addition(suite);
    test_vector_multiplication(suite);
    test_dot_product(suite);
    
    // Print results
    test_suite_print_results(suite);
    
    // Clean up
    int failed = suite->failed;
    test_suite_destroy(suite);
    
    return failed ? 1 : 0;
}

/**
 * test_basic_ops.c
 * Unit tests for basic vector operations
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/simd_ops.h"
#include "../include/neon_utils.h"
#include "../include/test_framework.h"
#include "../include/perf_test.h"

// Scalar implementation for addition
void scalar_add_f32(const float* a, const float* b, float* c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c[i] = a[i] + b[i];
    }
}

// Scalar implementation for multiplication
void scalar_mul_f32(const float* a, const float* b, float* c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        c[i] = a[i] * b[i];
    }
}

// Scalar implementation for dot product
float scalar_dot_product_f32(const float* a, const float* b, size_t len) {
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// Test vector addition
void test_vector_addition(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    float* c_simd = (float*)neon_malloc(test_size * sizeof(float));
    float* c_scalar = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b || !c_simd || !c_scalar) {
        test_suite_add_result(suite, "Vector Addition - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    simd_add_f32(a, b, c_simd, test_size);
    scalar_add_f32(a, b, c_scalar, test_size);
    
    // Verify results
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Results", c_simd, c_scalar, test_size, 1e-6f);
    
    // Test edge cases
    
    // 1. Test with unaligned size
    const size_t unaligned_size = test_size - 3;
    memset(c_simd, 0, test_size * sizeof(float));
    memset(c_scalar, 0, test_size * sizeof(float));
    
    simd_add_f32(a, b, c_simd, unaligned_size);
    scalar_add_f32(a, b, c_scalar, unaligned_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Unaligned Size", 
                          c_simd, c_scalar, unaligned_size, 1e-6f);
    
    // 2. Test with small arrays (smaller than SIMD width)
    const size_t small_size = 3;
    simd_add_f32(a, b, c_simd, small_size);
    scalar_add_f32(a, b, c_scalar, small_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Addition - Small Arrays", 
                          c_simd, c_scalar, small_size, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
    free(c_simd);
    free(c_scalar);
}

// Test vector multiplication
void test_vector_multiplication(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    float* c_simd = (float*)neon_malloc(test_size * sizeof(float));
    float* c_scalar = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b || !c_simd || !c_scalar) {
        test_suite_add_result(suite, "Vector Multiplication - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    simd_mul_f32(a, b, c_simd, test_size);
    scalar_mul_f32(a, b, c_scalar, test_size);
    
    // Verify results
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Multiplication - Results", 
                          c_simd, c_scalar, test_size, 1e-6f);
    
    // Test edge cases (similar to addition tests)
    const size_t unaligned_size = test_size - 3;
    memset(c_simd, 0, test_size * sizeof(float));
    memset(c_scalar, 0, test_size * sizeof(float));
    
    simd_mul_f32(a, b, c_simd, unaligned_size);
    scalar_mul_f32(a, b, c_scalar, unaligned_size);
    
    ASSERT_FLOAT_ARRAY_EQ(suite, "Vector Multiplication - Unaligned Size", 
                          c_simd, c_scalar, unaligned_size, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
    free(c_simd);
    free(c_scalar);
}

// Test dot product
void test_dot_product(test_suite_t* suite) {
    const size_t test_size = 1024;
    
    // Allocate aligned memory
    float* a = (float*)neon_malloc(test_size * sizeof(float));
    float* b = (float*)neon_malloc(test_size * sizeof(float));
    
    if (!a || !b) {
        test_suite_add_result(suite, "Dot Product - Allocation", false, "Memory allocation failed");
        return;
    }
    
    // Initialize test data
    for (size_t i = 0; i < test_size; i++) {
        a[i] = (float)i * 0.1f;
        b[i] = (float)(test_size - i) * 0.2f;
    }
    
    // Run operations
    float result_simd = simd_dot_product_f32(a, b, test_size);
    float result_scalar = scalar_dot_product_f32(a, b, test_size);
    
    // Verify results (allow small epsilon due to different summation order)
    ASSERT_FLOAT_EQ(suite, "Dot Product - Full Size", result_simd, result_scalar, 1e-4f);
    
    // Test edge cases
    const size_t unaligned_size = test_size - 3;
    float result_simd_unaligned = simd_dot_product_f32(a, b, unaligned_size);
    float result_scalar_unaligned = scalar_dot_product_f32(a, b, unaligned_size);
    
    ASSERT_FLOAT_EQ(suite, "Dot Product - Unaligned Size", 
                    result_simd_unaligned, result_scalar_unaligned, 1e-4f);
    
    // Test with small arrays
    const size_t small_size = 3;
    float result_simd_small = simd_dot_product_f32(a, b, small_size);
    float result_scalar_small = scalar_dot_product_f32(a, b, small_size);
    
    ASSERT_FLOAT_EQ(suite, "Dot Product - Small Arrays", 
                    result_simd_small, result_scalar_small, 1e-6f);
    
    // Cleanup
    free(a);
    free(b);
}

// Main test function
int main() {
    printf("Running unit tests for basic vector operations...\n");
    
    // Create test suite
    test_suite_t* suite = test_suite_create("Basic Vector Operations");
    
    // Run tests
    test_vector_addition(suite);
    test_vector_multiplication(suite);
    test_dot_product(suite);
    
    // Print results
    test_suite_print_results(suite);
    
    // Clean up
    int failed = suite->failed;
    test_suite_destroy(suite);
    
    return failed ? 1 : 0;
}
