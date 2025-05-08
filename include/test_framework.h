/**
 * test_framework.h
 * Simple unit testing framework for ARM NEON operations
 */
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Test result structure
typedef struct {
    const char* name;
    bool passed;
    char message[256];
} test_result_t;

// Test suite structure
typedef struct {
    const char* name;
    test_result_t* results;
    int count;
    int capacity;
    int passed;
    int failed;
} test_suite_t;

// Create a new test suite
static inline test_suite_t* test_suite_create(const char* name) {
    test_suite_t* suite = (test_suite_t*)malloc(sizeof(test_suite_t));
    if (!suite) return NULL;
    
    suite->name = name;
    suite->capacity = 32;  // Initial capacity
    suite->results = (test_result_t*)malloc(suite->capacity * sizeof(test_result_t));
    if (!suite->results) {
        free(suite);
        return NULL;
    }
    
    suite->count = 0;
    suite->passed = 0;
    suite->failed = 0;
    
    return suite;
}

// Add a test result to the suite
static inline void test_suite_add_result(test_suite_t* suite, const char* name, bool passed, const char* message) {
    if (suite->count >= suite->capacity) {
        // Expand capacity
        int new_capacity = suite->capacity * 2;
        test_result_t* new_results = (test_result_t*)realloc(suite->results, 
                                                            new_capacity * sizeof(test_result_t));
        if (!new_results) return;
        
        suite->results = new_results;
        suite->capacity = new_capacity;
    }
    
    test_result_t* result = &suite->results[suite->count++];
    result->name = name;
    result->passed = passed;
    strncpy(result->message, message, sizeof(result->message) - 1);
    result->message[sizeof(result->message) - 1] = '\0';
    
    if (passed) {
        suite->passed++;
    } else {
        suite->failed++;
    }
}

// Print test suite results
static inline void test_suite_print_results(const test_suite_t* suite) {
    printf("\n=== Test Suite: %s ===\n", suite->name);
    printf("Total: %d, Passed: %d, Failed: %d\n\n", suite->count, suite->passed, suite->failed);
    
    for (int i = 0; i < suite->count; i++) {
        const test_result_t* result = &suite->results[i];
        printf("[%s] %s: %s\n", 
               result->passed ? "PASS" : "FAIL",
               result->name,
               result->message);
    }
    
    printf("\nResult: %s\n", suite->failed == 0 ? "PASSED" : "FAILED");
}

// Free test suite
static inline void test_suite_destroy(test_suite_t* suite) {
    if (suite) {
        free(suite->results);
        free(suite);
    }
}

// Assert macros for different types
#define ASSERT_INT_EQ(suite, name, actual, expected) do { \
    bool passed = (actual) == (expected); \
    char message[256]; \
    if (passed) { \
        snprintf(message, sizeof(message), "Values match: %d", (actual)); \
    } else { \
        snprintf(message, sizeof(message), "Expected %d, got %d", (expected), (actual)); \
    } \
    test_suite_add_result(suite, name, passed, message); \
} while(0)

#define ASSERT_FLOAT_EQ(suite, name, actual, expected, epsilon) do { \
    bool passed = fabs((actual) - (expected)) <= (epsilon); \
    char message[256]; \
    if (passed) { \
        snprintf(message, sizeof(message), "Values approximately match: %f", (actual)); \
    } else { \
        snprintf(message, sizeof(message), "Expected %f (±%f), got %f", (expected), (epsilon), (actual)); \
    } \
    test_suite_add_result(suite, name, passed, message); \
} while(0)

#define ASSERT_ARRAY_EQ(suite, name, actual, expected, size, type, fmt) do { \
    bool passed = true; \
    int first_diff = -1; \
    for (int i = 0; i < (size); i++) { \
        if ((actual)[i] != (expected)[i]) { \
            passed = false; \
            first_diff = i; \
            break; \
        } \
    } \
    char message[256]; \
    if (passed) { \
        snprintf(message, sizeof(message), "Arrays match, size %d", (size)); \
    } else { \
        snprintf(message, sizeof(message), "Arrays differ at index %d: expected " fmt ", got " fmt, \
                first_diff, (expected)[first_diff], (actual)[first_diff]); \
    } \
    test_suite_add_result(suite, name, passed, message); \
} while(0)

#define ASSERT_FLOAT_ARRAY_EQ(suite, name, actual, expected, size, epsilon) do { \
    bool passed = true; \
    int first_diff = -1; \
    float max_diff = 0.0f; \
    for (int i = 0; i < (size); i++) { \
        float diff = fabs((actual)[i] - (expected)[i]); \
        if (diff > max_diff) max_diff = diff; \
        if (diff > (epsilon)) { \
            passed = false; \
            first_diff = i; \
            break; \
        } \
    } \
    char message[256]; \
    if (passed) { \
        snprintf(message, sizeof(message), "Arrays approximately match, max diff: %f", max_diff); \
    } else { \
        snprintf(message, sizeof(message), "Arrays differ at index %d: expected %f (±%f), got %f", \
                first_diff, (expected)[first_diff], (epsilon), (actual)[first_diff]); \
    } \
    test_suite_add_result(suite, name, passed, message); \
} while(0)

#endif /* TEST_FRAMEWORK_H */
