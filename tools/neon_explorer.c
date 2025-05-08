/**
 * neon_explorer.c
 * Interactive tool to experiment with ARM NEON instructions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arm_neon.h>
#include "../include/neon_utils.h"
#include "../include/platform_detect.h"

// Print usage information
void print_usage(void) {
    printf("\nNEON Explorer - Interactive ARM NEON Testing Tool\n");
    printf("Usage:\n");
    printf("  help               Show this help message\n");
    printf("  info               Show platform information\n");
    printf("  test float [op]    Test float operations (add, sub, mul, div, min, max)\n");
    printf("  test int [op]      Test integer operations (add, sub, mul, min, max)\n");
    printf("  test logicals      Test logical operations (and, or, xor, not)\n");
    printf("  test comparisons   Test comparison operations (eq, gt, lt, ge, le)\n");
    printf("  test shuffle       Test shuffling/rearrangement operations\n");
    printf("  time [op] [size]   Benchmark operation with given vector size\n");
    printf("  exit               Exit the program\n");
}

// Test float operations
void test_float_operations(const char* op) {
    printf("\n--- Testing Float Operations (%s) ---\n", op);
    
    // Create test vectors
    float32x4_t a = {1.0f, 2.0f, 3.0f, 4.0f};
    float32x4_t b = {5.0f, 6.0f, 7.0f, 8.0f};
    float32x4_t c;
    
    print_float32x4("a", a);
    print_float32x4("b", b);
    
    if (strcmp(op, "add") == 0 || strcmp(op, "") == 0) {
        c = vaddq_f32(a, b);
        print_float32x4("a + b", c);
    }
    
    if (strcmp(op, "sub") == 0 || strcmp(op, "") == 0) {
        c = vsubq_f32(a, b);
        print_float32x4("a - b", c);
    }
    
    if (strcmp(op, "mul") == 0 || strcmp(op, "") == 0) {
        c = vmulq_f32(a, b);
        print_float32x4("a * b", c);
    }
    
    if (strcmp(op, "div") == 0 || strcmp(op, "") == 0) {
        c = vdivq_f32(a, b);
        print_float32x4("a / b", c);
    }
    
    if (strcmp(op, "min") == 0 || strcmp(op, "") == 0) {
        c = vminq_f32(a, b);
        print_float32x4("min(a, b)", c);
    }
    
    if (strcmp(op, "max") == 0 || strcmp(op, "") == 0) {
        c = vmaxq_f32(a, b);
        print_float32x4("max(a, b)", c);
    }
}

// Test integer operations
void test_int_operations(const char* op) {
    printf("\n--- Testing Integer Operations (%s) ---\n", op);
    
    // Create test vectors
    int32x4_t a = {1, 2, 3, 4};
    int32x4_t b = {5, 6, 7, 8};
    int32x4_t c;
    
    print_int32x4("a", a);
    print_int32x4("b", b);
    
    if (strcmp(op, "add") == 0 || strcmp(op, "") == 0) {
        c = vaddq_s32(a, b);
        print_int32x4("a + b", c);
    }
    
    if (strcmp(op, "sub") == 0 || strcmp(op, "") == 0) {
        c = vsubq_s32(a, b);
        print_int32x4("a - b", c);
    }
    
    if (strcmp(op, "mul") == 0 || strcmp(op, "") == 0) {
        c = vmulq_s32(a, b);
        print_int32x4("a * b", c);
    }
    
    if (strcmp(op, "min") == 0 || strcmp(op, "") == 0) {
        c = vminq_s32(a, b);
        print_int32x4("min(a, b)", c);
    }
    
    if (strcmp(op, "max") == 0 || strcmp(op, "") == 0) {
        c = vmaxq_s32(a, b);
        print_int32x4("max(a, b)", c);
    }
}

// Test logical operations
void test_logical_operations(void) {
    printf("\n--- Testing Logical Operations ---\n");
    
    // Create test vectors
    uint32x4_t a = {0x0F0F0F0F, 0xAAAAAAAA, 0x00FF00FF, 0xFFFFFFFF};
    uint32x4_t b = {0xF0F0F0F0, 0x55555555, 0xFF00FF00, 0x00000000};
    uint32x4_t c;
    
    print_uint32x4("a", a);
    print_uint32x4("b", b);
    
    c = vandq_u32(a, b);
    print_uint32x4("a & b", c);
    
    c = vorrq_u32(a, b);
    print_uint32x4("a | b", c);
    
    c = veorq_u32(a, b);
    print_uint32x4("a ^ b", c);
    
    c = vmvnq_u32(a);
    print_uint32x4("~a", c);
}

// Test comparison operations
void test_comparison_operations(void) {
    printf("\n--- Testing Comparison Operations ---\n");
    
    // Create test vectors
    float32x4_t a = {1.0f, 6.0f, 3.0f, 8.0f};
    float32x4_t b = {5.0f, 2.0f, 3.0f, 4.0f};
    uint32x4_t c;
    
    print_float32x4("a", a);
    print_float32x4("b", b);
    
    c = vceqq_f32(a, b);
    print_uint32x4("a == b", c);
    
    c = vcgtq_f32(a, b);
    print_uint32x4("a > b", c);
    
    c = vcgeq_f32(a, b);
    print_uint32x4("a >= b", c);
    
    c = vcltq_f32(a, b);
    print_uint32x4("a < b", c);
    
    c = vcleq_f32(a, b);
    print_uint32x4("a <= b", c);
}

// Test shuffling operations
void test_shuffle_operations(void) {
    printf("\n--- Testing Shuffle/Rearrangement Operations ---\n");
    
    // Create test vectors
    float32x4_t a = {1.0f, 2.0f, 3.0f, 4.0f};
    float32x4_t b = {5.0f, 6.0f, 7.0f, 8.0f};
    
    print_float32x4("a", a);
    print_float32x4("b", b);
    
    // Interleave low
    float32x2_t a_low = vget_low_f32(a);
    float32x2_t b_low = vget_low_f32(b);
    print_float32x2("a_low", a_low);
    print_float32x2("b_low", b_low);
    
    float32x2x2_t interleaved = vzip_f32(a_low, b_low);
    print_float32x2("zip.val[0]", interleaved.val[0]);
    print_float32x2("zip.val[1]", interleaved.val[1]);
    
    // Extract elements
    float32x2_t extracted = vext_f32(a_low, b_low, 1);
    print_float32x2("ext(a_low, b_low, 1)", extracted);
    
    // Duplicate elements
    int16x8_t v16 = {1, 2, 3, 4, 5, 6, 7, 8};
    print_int16x8("v16", v16);
    
    int16x8_t rev = vrev64q_s16(v16);
    print_int16x8("rev64(v16)", rev);
}

// Benchmark operation
void benchmark_operation(const char* op, int size) {
    printf("\n--- Benchmarking %s operation with %d elements ---\n", op, size);
    
    // Allocate memory
    float* a = (float*)neon_malloc(size * sizeof(float));
    float* b = (float*)neon_malloc(size * sizeof(float));
    float* c = (float*)neon_malloc(size * sizeof(float));
    
    if (!a || !b || !c) {
        printf("ERROR: Memory allocation failed.\n");
        return;
    }
    
    // Initialize arrays
    for (int i = 0; i < size; i++) {
        a[i] = (float)i / 10.0f;
        b[i] = (float)(size - i) / 10.0f;
    }
    
    // Time NEON implementation
    uint64_t start_time = get_time_us();
    
    // Perform operation
    if (strcmp(op, "add") == 0) {
        for (int i = 0; i < size; i += 4) {
            float32x4_t va = vld1q_f32(a + i);
            float32x4_t vb = vld1q_f32(b + i);
            float32x4_t vc = vaddq_f32(va, vb);
            vst1q_f32(c + i, vc);
        }
    } else if (strcmp(op, "mul") == 0) {
        for (int i = 0; i < size; i += 4) {
            float32x4_t va = vld1q_f32(a + i);
            float32x4_t vb = vld1q_f32(b + i);
            float32x4_t vc = vmulq_f32(va, vb);
            vst1q_f32(c + i, vc);
        }
    } else {
        printf("Unknown operation: %s\n", op);
    }
    
    uint64_t neon_time = get_time_us() - start_time;
    
    // Time scalar implementation
    start_time = get_time_us();
    
    if (strcmp(op, "add") == 0) {
        for (int i = 0; i < size; i++) {
            c[i] = a[i] + b[i];
        }
    } else if (strcmp(op, "mul") == 0) {
        for (int i = 0; i < size; i++) {
            c[i] = a[i] * b[i];
        }
    }
    
    uint64_t scalar_time = get_time_us() - start_time;
    
    // Print results
    printf("NEON time: %lu us\n", (unsigned long)neon_time);
    printf("Scalar time: %lu us\n", (unsigned long)scalar_time);
    printf("Speedup: %.2fx\n", (double)scalar_time / neon_time);
    
    // Clean up
    free(a);
    free(b);
    free(c);
}

// Main function
int main() {
    char cmd[256];
    
    // Check if NEON is supported
    if (!check_neon_support()) {
        printf("ERROR: ARM NEON is not supported on this platform.\n");
        return 1;
    }
    
    printf("ARM NEON Explorer - Interactive Testing Tool\n");
    printf("Type 'help' for available commands\n");
    
    while (1) {
        printf("\n> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        cmd[strcspn(cmd, "\n")] = 0;
        
        // Parse command
        char* token = strtok(cmd, " ");
        if (!token) continue;
        
        if (strcmp(token, "help") == 0) {
            print_usage();
        } else if (strcmp(token, "info") == 0) {
            print_platform_info();
        } else if (strcmp(token, "test") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                printf("Missing test type. Use 'help' for usage information.\n");
                continue;
            }
            
            if (strcmp(token, "float") == 0) {
                token = strtok(NULL, " ");
                test_float_operations(token ? token : "");
            } else if (strcmp(token, "int") == 0) {
                token = strtok(NULL, " ");
                test_int_operations(token ? token : "");
            } else if (strcmp(token, "logicals") == 0) {
                test_logical_operations();
            } else if (strcmp(token, "comparisons") == 0) {
                test_comparison_operations();
            } else if (strcmp(token, "shuffle") == 0) {
                test_shuffle_operations();
            } else {
                printf("Unknown test type: %s\n", token);
            }
        } else if (strcmp(token, "time") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                printf("Missing operation. Use 'help' for usage information.\n");
                continue;
            }
            
            char* op = token;
            token = strtok(NULL, " ");
            int size = token ? atoi(token) : 1000000;
            
            benchmark_operation(op, size);
        } else if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", token);
            print_usage();
        }
    }
    
    return 0;
}
