/**
 * matrix_multiply.c
 * Demonstrates matrix multiplication using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <arm_neon.h>
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Matrix structure
typedef struct {
    float* data;
    int rows;
    int cols;
} matrix_t;

// Create a new matrix
matrix_t* matrix_create(int rows, int cols) {
    matrix_t* mat = (matrix_t*)malloc(sizeof(matrix_t));
    if (!mat) return NULL;
    
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)neon_malloc(rows * cols * sizeof(float));
    
    if (!mat->data) {
        free(mat);
        return NULL;
    }
    
    return mat;
}

// Initialize matrix with random values
void matrix_randomize(matrix_t* mat, float min_val, float max_val) {
    float range = max_val - min_val;
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            float r = (float)rand() / RAND_MAX;
            mat->data[i * mat->cols + j] = min_val + r * range;
        }
    }
}

// Print matrix
void matrix_print(const matrix_t* mat, const char* name) {
    printf("%s (%d x %d):\n", name, mat->rows, mat->cols);
    
    // Print up to 6x6 elements
    int show_rows = (mat->rows > 6) ? 6 : mat->rows;
    int show_cols = (mat->cols > 6) ? 6 : mat->cols;
    
    for (int i = 0; i < show_rows; i++) {
        for (int j = 0; j < show_cols; j++) {
            printf("%8.2f ", mat->data[i * mat->cols + j]);
        }
        
        if (mat->cols > show_cols) {
            printf("...");
        }
        printf("\n");
    }
    
    if (mat->rows > show_rows) {
        printf("...\n");
    }
    printf("\n");
}

// Free matrix
void matrix_destroy(matrix_t* mat) {
    if (mat) {
        free(mat->data);
        free(mat);
    }
}

// Matrix multiplication (scalar version)
void matrix_multiply_scalar(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    if (a->cols != b->rows || c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: Incompatible matrix dimensions\n");
        return;
    }
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            c->data[i * c->cols + j] = sum;
        }
    }
}

// Matrix multiplication using NEON SIMD
void matrix_multiply_neon(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    if (a->cols != b->rows || c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: Incompatible matrix dimensions\n");
        return;
    }
    
    // Compute each row of the result
    for (int i = 0; i < a->rows; i++) {
        // For each column of B
        for (int j = 0; j < b->cols; j += 4) {
            // Handle case where b->cols is not a multiple of 4
            if (j + 4 > b->cols) {
                // Process remaining columns one by one
                for (int jj = j; jj < b->cols; jj++) {
                    float sum = 0.0f;
                    for (int k = 0; k < a->cols; k++) {
                        sum += a->data[i * a->cols + k] * b->data[k * b->cols + jj];
                    }
                    c->data[i * c->cols + jj] = sum;
                }
                break;
            }
            
            // Initialize accumulator to zero
            float32x4_t sum_vec = vdupq_n_f32(0.0f);
            
            // Compute dot product of row i of A with columns j,j+1,j+2,j+3 of B
            for (int k = 0; k < a->cols; k++) {
                // Load 1 element from A and broadcast to all lanes
                float32x4_t a_elem = vdupq_n_f32(a->data[i * a->cols + k]);
                
                // Load 4 elements from the current row of B
                float32x4_t b_elems = vld1q_f32(&b->data[k * b->cols + j]);
                
                // Multiply and accumulate
                sum_vec = vmlaq_f32(sum_vec, a_elem, b_elems);
            }
            
            // Store the result in C
            vst1q_f32(&c->data[i * c->cols + j], sum_vec);
        }
    }
}

// Verify matrices are equal (within epsilon)
bool matrix_equals(const matrix_t* a, const matrix_t* b, float epsilon) {
    if (a->rows != b->rows || a->cols != b->cols) {
        return false;
    }
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            float diff = fabsf(a->data[i * a->cols + j] - b->data[i * a->cols + j]);
            if (diff > epsilon) {
                return false;
            }
        }
    }
    
    return true;
}

int main(int argc, char** argv) {
    // Default matrix dimensions
    int a_rows = 128;
    int a_cols = 128;
    int b_rows = a_cols;  // Must match for multiplication
    int b_cols = 128;
    
    // Allow overriding matrix dimensions from command line
    if (argc > 3) {
        a_rows = atoi(argv[1]);
        a_cols = atoi(argv[2]);
        b_cols = atoi(argv[3]);
        b_rows = a_cols;  // Must match for multiplication
        
        if (a_rows <= 0 || a_cols <= 0 || b_cols <= 0) {
            fprintf(stderr, "Error: Invalid matrix dimensions\n");
            return 1;
        }
    }
    
    printf("Matrix Multiplication Example\n");
    printf("----------------------------\n");
    printf("Matrix A: %d x %d\n", a_rows, a_cols);
    printf("Matrix B: %d x %d\n", b_rows, b_cols);
    printf("Result C: %d x %d\n\n", a_rows, b_cols);
    
    // Seed random number generator
    srand(time(NULL));
    
    // Create matrices
    matrix_t* a = matrix_create(a_rows, a_cols);
    matrix_t* b = matrix_create(b_rows, b_cols);
    matrix_t* c_scalar = matrix_create(a_rows, b_cols);
    matrix_t* c_neon = matrix_create(a_rows, b_cols);
    
    if (!a || !b || !c_scalar || !c_neon) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Initialize matrices with random values
    matrix_randomize(a, -1.0f, 1.0f);
    matrix_randomize(b, -1.0f, 1.0f);
    
    // Print small portions of the matrices
    if (a_rows <= 20 && a_cols <= 20 && b_cols <= 20) {
        matrix_print(a, "Matrix A");
        matrix_print(b, "Matrix B");
    }
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Matrix Multiplication");
    
    // Perform matrix multiplication using scalar implementation
    timer_start(comp->scalar_timer);
    matrix_multiply_scalar(a, b, c_scalar);
    timer_stop(comp->scalar_timer);
    
    // Perform matrix multiplication using NEON SIMD
    timer_start(comp->simd_timer);
    matrix_multiply_neon(a, b, c_neon);
    timer_stop(comp->simd_timer);
    
    // Verify results
    bool result_ok = matrix_equals(c_scalar, c_neon, 1e-5);
    
    printf("Verification: %s\n", result_ok ? "PASSED" : "FAILED");
    
    // Print small portions of the result
    if (a_rows <= 20 && b_cols <= 20) {
        matrix_print(c_neon, "Result Matrix C");
    }
    
    // Print performance comparison
    comparison_print(comp);
    
    // Calculate and print FLOPS (Floating Point Operations Per Second)
    uint64_t operations = 2ULL * a_rows * a_cols * b_cols;  // 2 operations per multiply-add
    double scalar_seconds = comp->scalar_timer->total_time / 1000000.0;
    double simd_seconds = comp->simd_timer->total_time / 1000000.0;
    
    double scalar_gflops = (operations / scalar_seconds) / 1e9;
    double simd_gflops = (operations / simd_seconds) / 1e9;
    
    printf("Scalar performance: %.2f GFLOPS\n", scalar_gflops);
    printf("NEON performance: %.2f GFLOPS\n", simd_gflops);
    
    // Clean up
    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c_scalar);
    matrix_destroy(c_neon);
    comparison_destroy(comp);
    
    return result_ok ? 0 : 1;
}

/**
 * matrix_multiply.c
 * Demonstrates matrix multiplication using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <arm_neon.h>
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Matrix structure
typedef struct {
    float* data;
    int rows;
    int cols;
} matrix_t;

// Create a new matrix
matrix_t* matrix_create(int rows, int cols) {
    matrix_t* mat = (matrix_t*)malloc(sizeof(matrix_t));
    if (!mat) return NULL;
    
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)neon_malloc(rows * cols * sizeof(float));
    
    if (!mat->data) {
        free(mat);
        return NULL;
    }
    
    return mat;
}

// Initialize matrix with random values
void matrix_randomize(matrix_t* mat, float min_val, float max_val) {
    float range = max_val - min_val;
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            float r = (float)rand() / RAND_MAX;
            mat->data[i * mat->cols + j] = min_val + r * range;
        }
    }
}

// Print matrix
void matrix_print(const matrix_t* mat, const char* name) {
    printf("%s (%d x %d):\n", name, mat->rows, mat->cols);
    
    // Print up to 6x6 elements
    int show_rows = (mat->rows > 6) ? 6 : mat->rows;
    int show_cols = (mat->cols > 6) ? 6 : mat->cols;
    
    for (int i = 0; i < show_rows; i++) {
        for (int j = 0; j < show_cols; j++) {
            printf("%8.2f ", mat->data[i * mat->cols + j]);
        }
        
        if (mat->cols > show_cols) {
            printf("...");
        }
        printf("\n");
    }
    
    if (mat->rows > show_rows) {
        printf("...\n");
    }
    printf("\n");
}

// Free matrix
void matrix_destroy(matrix_t* mat) {
    if (mat) {
        free(mat->data);
        free(mat);
    }
}

// Matrix multiplication (scalar version)
void matrix_multiply_scalar(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    if (a->cols != b->rows || c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: Incompatible matrix dimensions\n");
        return;
    }
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            c->data[i * c->cols + j] = sum;
        }
    }
}

// Matrix multiplication using NEON SIMD
void matrix_multiply_neon(const matrix_t* a, const matrix_t* b, matrix_t* c) {
    if (a->cols != b->rows || c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: Incompatible matrix dimensions\n");
        return;
    }
    
    // Compute each row of the result
    for (int i = 0; i < a->rows; i++) {
        // For each column of B
        for (int j = 0; j < b->cols; j += 4) {
            // Handle case where b->cols is not a multiple of 4
            if (j + 4 > b->cols) {
                // Process remaining columns one by one
                for (int jj = j; jj < b->cols; jj++) {
                    float sum = 0.0f;
                    for (int k = 0; k < a->cols; k++) {
                        sum += a->data[i * a->cols + k] * b->data[k * b->cols + jj];
                    }
                    c->data[i * c->cols + jj] = sum;
                }
                break;
            }
            
            // Initialize accumulator to zero
            float32x4_t sum_vec = vdupq_n_f32(0.0f);
            
            // Compute dot product of row i of A with columns j,j+1,j+2,j+3 of B
            for (int k = 0; k < a->cols; k++) {
                // Load 1 element from A and broadcast to all lanes
                float32x4_t a_elem = vdupq_n_f32(a->data[i * a->cols + k]);
                
                // Load 4 elements from the current row of B
                float32x4_t b_elems = vld1q_f32(&b->data[k * b->cols + j]);
                
                // Multiply and accumulate
                sum_vec = vmlaq_f32(sum_vec, a_elem, b_elems);
            }
            
            // Store the result in C
            vst1q_f32(&c->data[i * c->cols + j], sum_vec);
        }
    }
}

// Verify matrices are equal (within epsilon)
bool matrix_equals(const matrix_t* a, const matrix_t* b, float epsilon) {
    if (a->rows != b->rows || a->cols != b->cols) {
        return false;
    }
    
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            float diff = fabsf(a->data[i * a->cols + j] - b->data[i * a->cols + j]);
            if (diff > epsilon) {
                return false;
            }
        }
    }
    
    return true;
}

int main(int argc, char** argv) {
    // Default matrix dimensions
    int a_rows = 128;
    int a_cols = 128;
    int b_rows = a_cols;  // Must match for multiplication
    int b_cols = 128;
    
    // Allow overriding matrix dimensions from command line
    if (argc > 3) {
        a_rows = atoi(argv[1]);
        a_cols = atoi(argv[2]);
        b_cols = atoi(argv[3]);
        b_rows = a_cols;  // Must match for multiplication
        
        if (a_rows <= 0 || a_cols <= 0 || b_cols <= 0) {
            fprintf(stderr, "Error: Invalid matrix dimensions\n");
            return 1;
        }
    }
    
    printf("Matrix Multiplication Example\n");
    printf("----------------------------\n");
    printf("Matrix A: %d x %d\n", a_rows, a_cols);
    printf("Matrix B: %d x %d\n", b_rows, b_cols);
    printf("Result C: %d x %d\n\n", a_rows, b_cols);
    
    // Seed random number generator
    srand(time(NULL));
    
    // Create matrices
    matrix_t* a = matrix_create(a_rows, a_cols);
    matrix_t* b = matrix_create(b_rows, b_cols);
    matrix_t* c_scalar = matrix_create(a_rows, b_cols);
    matrix_t* c_neon = matrix_create(a_rows, b_cols);
    
    if (!a || !b || !c_scalar || !c_neon) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Initialize matrices with random values
    matrix_randomize(a, -1.0f, 1.0f);
    matrix_randomize(b, -1.0f, 1.0f);
    
    // Print small portions of the matrices
    if (a_rows <= 20 && a_cols <= 20 && b_cols <= 20) {
        matrix_print(a, "Matrix A");
        matrix_print(b, "Matrix B");
    }
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("Matrix Multiplication");
    
    // Perform matrix multiplication using scalar implementation
    timer_start(comp->scalar_timer);
    matrix_multiply_scalar(a, b, c_scalar);
    timer_stop(comp->scalar_timer);
    
    // Perform matrix multiplication using NEON SIMD
    timer_start(comp->simd_timer);
    matrix_multiply_neon(a, b, c_neon);
    timer_stop(comp->simd_timer);
    
    // Verify results
    bool result_ok = matrix_equals(c_scalar, c_neon, 1e-5);
    
    printf("Verification: %s\n", result_ok ? "PASSED" : "FAILED");
    
    // Print small portions of the result
    if (a_rows <= 20 && b_cols <= 20) {
        matrix_print(c_neon, "Result Matrix C");
    }
    
    // Print performance comparison
    comparison_print(comp);
    
    // Calculate and print FLOPS (Floating Point Operations Per Second)
    uint64_t operations = 2ULL * a_rows * a_cols * b_cols;  // 2 operations per multiply-add
    double scalar_seconds = comp->scalar_timer->total_time / 1000000.0;
    double simd_seconds = comp->simd_timer->total_time / 1000000.0;
    
    double scalar_gflops = (operations / scalar_seconds) / 1e9;
    double simd_gflops = (operations / simd_seconds) / 1e9;
    
    printf("Scalar performance: %.2f GFLOPS\n", scalar_gflops);
    printf("NEON performance: %.2f GFLOPS\n", simd_gflops);
    
    // Clean up
    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c_scalar);
    matrix_destroy(c_neon);
    comparison_destroy(comp);
    
    return result_ok ? 0 : 1;
}
