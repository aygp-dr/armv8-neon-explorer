/**
 * benchmark_config.h
 * Configuration and utilities for benchmarking NEON operations
 */
#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "perf_test.h"

// Benchmark configuration structure
typedef struct {
    const char* name;
    size_t min_size;
    size_t max_size;
    size_t step_factor;  // Multiplicative step (e.g., 2 doubles the size each step)
    int iterations;      // Number of iterations for each data size
    bool verify_results;
    const char* output_file;
} benchmark_config_t;

// Default benchmark configuration
static inline benchmark_config_t benchmark_config_default(const char* name) {
    benchmark_config_t config;
    config.name = name;
    config.min_size = 16;        // Start with small arrays
    config.max_size = 16777216;  // Up to 16M elements (64MB for float32)
    config.step_factor = 4;      // Multiply size by 4 each step
    config.iterations = 10;      // 10 iterations per data size
    config.verify_results = true;
    config.output_file = NULL;   // No file output by default
    return config;
}

// Benchmark results structure
typedef struct {
    size_t data_size;
    double simd_time_us;
    double scalar_time_us;
    double speedup;
} benchmark_result_t;

// Benchmark results array
typedef struct {
    benchmark_result_t* results;
    size_t count;
    size_t capacity;
    const char* name;
} benchmark_results_t;

// Create new benchmark results
static inline benchmark_results_t* benchmark_results_create(const char* name) {
    benchmark_results_t* results = (benchmark_results_t*)malloc(sizeof(benchmark_results_t));
    if (!results) return NULL;
    
    results->capacity = 32;  // Initial capacity
    results->results = (benchmark_result_t*)malloc(results->capacity * sizeof(benchmark_result_t));
    if (!results->results) {
        free(results);
        return NULL;
    }
    
    results->count = 0;
    results->name = name;
    
    return results;
}

// Add a benchmark result
static inline void benchmark_results_add(benchmark_results_t* results, 
                                        size_t data_size,
                                        double simd_time_us,
                                        double scalar_time_us) {
    if (results->count >= results->capacity) {
        // Expand capacity
        size_t new_capacity = results->capacity * 2;
        benchmark_result_t* new_results = (benchmark_result_t*)realloc(
            results->results, new_capacity * sizeof(benchmark_result_t));
        if (!new_results) return;
        
        results->results = new_results;
        results->capacity = new_capacity;
    }
    
    benchmark_result_t* result = &results->results[results->count++];
    result->data_size = data_size;
    result->simd_time_us = simd_time_us;
    result->scalar_time_us = scalar_time_us;
    result->speedup = scalar_time_us / simd_time_us;
}

// Print benchmark results
static inline void benchmark_results_print(benchmark_results_t* results) {
    printf("\n=== Benchmark Results: %s ===\n", results->name);
    printf("%-12s %-15s %-15s %-10s\n", 
           "Size", "SIMD Time (µs)", "Scalar Time (µs)", "Speedup");
    printf("-----------------------------------------------------------\n");
    
    for (size_t i = 0; i < results->count; i++) {
        benchmark_result_t* result = &results->results[i];
        printf("%-12zu %-15.2f %-15.2f %-10.2f\n",
               result->data_size,
               result->simd_time_us,
               result->scalar_time_us,
               result->speedup);
    }
    
    // Calculate average speedup
    double total_speedup = 0.0;
    for (size_t i = 0; i < results->count; i++) {
        total_speedup += results->results[i].speedup;
    }
    double avg_speedup = total_speedup / results->count;
    
    printf("-----------------------------------------------------------\n");
    printf("Average Speedup: %.2fx\n", avg_speedup);
}

// Save benchmark results to CSV file
static inline void benchmark_results_save_csv(benchmark_results_t* results, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }
    
    // Write header
    fprintf(file, "Size,SIMD Time (µs),Scalar Time (µs),Speedup\n");
    
    // Write data
    for (size_t i = 0; i < results->count; i++) {
        benchmark_result_t* result = &results->results[i];
        fprintf(file, "%zu,%.2f,%.2f,%.2f\n",
                result->data_size,
                result->simd_time_us,
                result->scalar_time_us,
                result->speedup);
    }
    
    fclose(file);
    printf("Results saved to %s\n", filename);
}

// Free benchmark results
static inline void benchmark_results_destroy(benchmark_results_t* results) {
    if (results) {
        free(results->results);
        free(results);
    }
}

#endif /* BENCHMARK_CONFIG_H */

/**
 * benchmark_config.h
 * Configuration and utilities for benchmarking NEON operations
 */
#ifndef BENCHMARK_CONFIG_H
#define BENCHMARK_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "perf_test.h"

// Benchmark configuration structure
typedef struct {
    const char* name;
    size_t min_size;
    size_t max_size;
    size_t step_factor;  // Multiplicative step (e.g., 2 doubles the size each step)
    int iterations;      // Number of iterations for each data size
    bool verify_results;
    const char* output_file;
} benchmark_config_t;

// Default benchmark configuration
static inline benchmark_config_t benchmark_config_default(const char* name) {
    benchmark_config_t config;
    config.name = name;
    config.min_size = 16;        // Start with small arrays
    config.max_size = 16777216;  // Up to 16M elements (64MB for float32)
    config.step_factor = 4;      // Multiply size by 4 each step
    config.iterations = 10;      // 10 iterations per data size
    config.verify_results = true;
    config.output_file = NULL;   // No file output by default
    return config;
}

// Benchmark results structure
typedef struct {
    size_t data_size;
    double simd_time_us;
    double scalar_time_us;
    double speedup;
} benchmark_result_t;

// Benchmark results array
typedef struct {
    benchmark_result_t* results;
    size_t count;
    size_t capacity;
    const char* name;
} benchmark_results_t;

// Create new benchmark results
static inline benchmark_results_t* benchmark_results_create(const char* name) {
    benchmark_results_t* results = (benchmark_results_t*)malloc(sizeof(benchmark_results_t));
    if (!results) return NULL;
    
    results->capacity = 32;  // Initial capacity
    results->results = (benchmark_result_t*)malloc(results->capacity * sizeof(benchmark_result_t));
    if (!results->results) {
        free(results);
        return NULL;
    }
    
    results->count = 0;
    results->name = name;
    
    return results;
}

// Add a benchmark result
static inline void benchmark_results_add(benchmark_results_t* results, 
                                        size_t data_size,
                                        double simd_time_us,
                                        double scalar_time_us) {
    if (results->count >= results->capacity) {
        // Expand capacity
        size_t new_capacity = results->capacity * 2;
        benchmark_result_t* new_results = (benchmark_result_t*)realloc(
            results->results, new_capacity * sizeof(benchmark_result_t));
        if (!new_results) return;
        
        results->results = new_results;
        results->capacity = new_capacity;
    }
    
    benchmark_result_t* result = &results->results[results->count++];
    result->data_size = data_size;
    result->simd_time_us = simd_time_us;
    result->scalar_time_us = scalar_time_us;
    result->speedup = scalar_time_us / simd_time_us;
}

// Print benchmark results
static inline void benchmark_results_print(benchmark_results_t* results) {
    printf("\n=== Benchmark Results: %s ===\n", results->name);
    printf("%-12s %-15s %-15s %-10s\n", 
           "Size", "SIMD Time (µs)", "Scalar Time (µs)", "Speedup");
    printf("-----------------------------------------------------------\n");
    
    for (size_t i = 0; i < results->count; i++) {
        benchmark_result_t* result = &results->results[i];
        printf("%-12zu %-15.2f %-15.2f %-10.2f\n",
               result->data_size,
               result->simd_time_us,
               result->scalar_time_us,
               result->speedup);
    }
    
    // Calculate average speedup
    double total_speedup = 0.0;
    for (size_t i = 0; i < results->count; i++) {
        total_speedup += results->results[i].speedup;
    }
    double avg_speedup = total_speedup / results->count;
    
    printf("-----------------------------------------------------------\n");
    printf("Average Speedup: %.2fx\n", avg_speedup);
}

// Save benchmark results to CSV file
static inline void benchmark_results_save_csv(benchmark_results_t* results, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }
    
    // Write header
    fprintf(file, "Size,SIMD Time (µs),Scalar Time (µs),Speedup\n");
    
    // Write data
    for (size_t i = 0; i < results->count; i++) {
        benchmark_result_t* result = &results->results[i];
        fprintf(file, "%zu,%.2f,%.2f,%.2f\n",
                result->data_size,
                result->simd_time_us,
                result->scalar_time_us,
                result->speedup);
    }
    
    fclose(file);
    printf("Results saved to %s\n", filename);
}

// Free benchmark results
static inline void benchmark_results_destroy(benchmark_results_t* results) {
    if (results) {
        free(results->results);
        free(results);
    }
}

#endif /* BENCHMARK_CONFIG_H */
