/**
 * fft_example.c
 * Demonstrates Fast Fourier Transform using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <arm_neon.h>
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Define complex number structure for NEON operations
typedef struct {
    float re;
    float im;
} complex_t;

// Complex number operations
static inline complex_t complex_add(complex_t a, complex_t b) {
    complex_t result = { a.re + b.re, a.im + b.im };
    return result;
}

static inline complex_t complex_sub(complex_t a, complex_t b) {
    complex_t result = { a.re - b.re, a.im - b.im };
    return result;
}

static inline complex_t complex_mul(complex_t a, complex_t b) {
    complex_t result = { 
        a.re * b.re - a.im * b.im,
        a.re * b.im + a.im * b.re
    };
    return result;
}

// Generate twiddle factors for FFT
void generate_twiddle_factors(complex_t* twiddle, int n) {
    for (int k = 0; k < n / 2; k++) {
        float angle = -2.0f * M_PI * k / n;
        twiddle[k].re = cosf(angle);
        twiddle[k].im = sinf(angle);
    }
}

// Bit-reverse an index for FFT
int bit_reverse(int x, int bits) {
    int result = 0;
    for (int i = 0; i < bits; i++) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

// Reorder array in bit-reversed order
void bit_reverse_array(complex_t* array, int n) {
    int bits = 0;
    int temp = n;
    while (temp > 1) {
        bits++;
        temp >>= 1;
    }
    
    for (int i = 0; i < n; i++) {
        int rev = bit_reverse(i, bits);
        if (i < rev) {
            complex_t tmp = array[i];
            array[i] = array[rev];
            array[rev] = tmp;
        }
    }
}

// Scalar implementation of Radix-2 FFT
void fft_scalar(complex_t* x, int n, complex_t* twiddle) {
    // Reorder input in bit-reversed order
    bit_reverse_array(x, n);
    
    // Perform FFT
    for (int stage = 1; stage <= log2f(n); stage++) {
        int m = 1 << stage;
        int m2 = m >> 1;
        
        for (int k = 0; k < n; k += m) {
            for (int j = 0; j < m2; j++) {
                complex_t t = complex_mul(x[k + j + m2], twiddle[j * n / m]);
                complex_t u = x[k + j];
                x[k + j] = complex_add(u, t);
                x[k + j + m2] = complex_sub(u, t);
            }
        }
    }
}

// NEON implementation of Radix-2 FFT
void fft_neon(complex_t* x, int n, complex_t* twiddle) {
    // Reorder input in bit-reversed order
    bit_reverse_array(x, n);
    
    // Perform FFT using NEON for butterfly operations
    for (int stage = 1; stage <= log2f(n); stage++) {
        int m = 1 << stage;
        int m2 = m >> 1;
        
        for (int k = 0; k < n; k += m) {
            for (int j = 0; j < m2; j += 2) {
                // Process two butterflies at once if possible
                if (j + 1 < m2) {
                    // Load twiddle factors
                    float32x4_t tw = vld1q_f32((float*)&twiddle[j * n / m]);
                    
                    // Load input values
                    float32x4_t a = vld1q_f32((float*)&x[k + j]);
                    float32x4_t b = vld1q_f32((float*)&x[k + j + m2]);
                    
                    // Perform complex multiplication using NEON
                    // (a+bi) * (c+di) = (ac-bd) + (ad+bc)i
                    float32x4_t tw_flip = vcombine_f32(
                        vext_f32(vget_low_f32(tw), vget_high_f32(tw), 1),
                        vext_f32(vget_low_f32(tw), vget_high_f32(tw), 1)
                    );
                    
                    // Negate imaginary part for complex multiplication
                    float32x4_t tw_conj = vreinterpretq_f32_u32(
                        veorq_u32(
                            vreinterpretq_u32_f32(tw_flip),
                            vreinterpretq_u32_f32(vdupq_n_f32(-0.0f))
                        )
                    );
                    
                    // Multiply real and swapped components
                    float32x4_t prod1 = vmulq_f32(b, tw);
                    float32x4_t prod2 = vmulq_f32(b, tw_conj);
                    
                    // Shuffle to get correct order
                    float32x4_t prod = vtrn1q_f32(prod1, prod2);
                    
                    // Compute butterfly
                    float32x4_t sum = vaddq_f32(a, prod);
                    float32x4_t diff = vsubq_f32(a, prod);
                    
                    // Store results
                    vst1q_f32((float*)&x[k + j], sum);
                    vst1q_f32((float*)&x[k + j + m2], diff);
                } else {
                    // Handle remaining butterfly (if odd number)
                    complex_t t = complex_mul(x[k + j + m2], twiddle[j * n / m]);
                    complex_t u = x[k + j];
                    x[k + j] = complex_add(u, t);
                    x[k + j + m2] = complex_sub(u, t);
                }
            }
        }
    }
}

// Generate a test signal (sum of sine waves)
void generate_test_signal(complex_t* signal, int n) {
    for (int i = 0; i < n; i++) {
        float t = (float)i / n;
        float val = 0.5f * sinf(2.0f * M_PI * 5.0f * t) +  // 5 Hz component
                    0.3f * sinf(2.0f * M_PI * 10.0f * t) + // 10 Hz component
                    0.1f * sinf(2.0f * M_PI * 20.0f * t);  // 20 Hz component
        signal[i].re = val;
        signal[i].im = 0.0f;
    }
}

// Print complex array (for debugging)
void print_complex_array(const complex_t* array, int n, const char* name) {
    printf("%s:\n", name);
    int print_count = (n > 16) ? 16 : n;  // Print at most 16 elements
    
    for (int i = 0; i < print_count; i++) {
        printf("[%2d] %8.4f%+8.4fi\n", i, array[i].re, array[i].im);
    }
    
    if (n > print_count) {
        printf("...\n");
    }
    printf("\n");
}

// Calculate magnitude of FFT output
void calculate_magnitude(const complex_t* fft, float* magnitude, int n) {
    for (int i = 0; i < n; i++) {
        magnitude[i] = sqrtf(fft[i].re * fft[i].re + fft[i].im * fft[i].im);
    }
}

// Verify two FFT results are approximately equal
bool verify_fft_results(const complex_t* a, const complex_t* b, int n, float epsilon) {
    for (int i = 0; i < n; i++) {
        float diff_re = fabsf(a[i].re - b[i].re);
        float diff_im = fabsf(a[i].im - b[i].im);
        
        if (diff_re > epsilon || diff_im > epsilon) {
            printf("Mismatch at index %d: (%f,%f) vs (%f,%f)\n",
                i, a[i].re, a[i].im, b[i].re, b[i].im);
            return false;
        }
    }
    
    return true;
}

int main(int argc, char** argv) {
    // Default FFT size (power of 2)
    int fft_size = 1024;
    
    // Allow overriding FFT size from command line
    if (argc > 1) {
        fft_size = atoi(argv[1]);
        
        // Ensure FFT size is a power of 2
        int log2_size = (int)log2f(fft_size);
        if (fft_size != (1 << log2_size)) {
            fprintf(stderr, "Error: FFT size must be a power of 2\n");
            return 1;
        }
    }
    
    printf("FFT Example\n");
    printf("-----------\n");
    printf("FFT Size: %d\n\n", fft_size);
    
    // Allocate memory for input/output
    complex_t* signal = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    complex_t* output_scalar = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    complex_t* output_neon = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    float* magnitude = (float*)neon_malloc(fft_size * sizeof(float));
    complex_t* twiddle = (complex_t*)neon_malloc(fft_size/2 * sizeof(complex_t));
    
    if (!signal || !output_scalar || !output_neon || !magnitude || !twiddle) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Generate test signal
    generate_test_signal(signal, fft_size);
    
    // Generate twiddle factors
    generate_twiddle_factors(twiddle, fft_size);
    
    // Copy input signal to output buffers
    memcpy(output_scalar, signal, fft_size * sizeof(complex_t));
    memcpy(output_neon, signal, fft_size * sizeof(complex_t));
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("FFT");
    
    // Run scalar FFT
    timer_start(comp->scalar_timer);
    fft_scalar(output_scalar, fft_size, twiddle);
    timer_stop(comp->scalar_timer);
    
    // Run NEON FFT
    timer_start(comp->simd_timer);
    fft_neon(output_neon, fft_size, twiddle);
    timer_stop(comp->simd_timer);
    
    // Verify results
    bool result_ok = verify_fft_results(output_scalar, output_neon, fft_size, 1e-3f);
    printf("Verification: %s\n", result_ok ? "PASSED" : "FAILED");
    
    // Calculate and print magnitude spectrum (first few frequencies)
    calculate_magnitude(output_neon, magnitude, fft_size);
    
    printf("Magnitude Spectrum (first 10 frequencies):\n");
    for (int i = 0; i < 10; i++) {
        float freq = (float)i * fft_size / fft_size;
        printf("[%2d] %8.2f Hz: %8.4f\n", i, freq, magnitude[i]);
    }
    printf("\n");
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(signal);
    free(output_scalar);
    free(output_neon);
    free(magnitude);
    free(twiddle);
    comparison_destroy(comp);
    
    return result_ok ? 0 : 1;
}

/**
 * fft_example.c
 * Demonstrates Fast Fourier Transform using ARM NEON SIMD
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <arm_neon.h>
#include "../include/neon_utils.h"
#include "../include/perf_test.h"

// Define complex number structure for NEON operations
typedef struct {
    float re;
    float im;
} complex_t;

// Complex number operations
static inline complex_t complex_add(complex_t a, complex_t b) {
    complex_t result = { a.re + b.re, a.im + b.im };
    return result;
}

static inline complex_t complex_sub(complex_t a, complex_t b) {
    complex_t result = { a.re - b.re, a.im - b.im };
    return result;
}

static inline complex_t complex_mul(complex_t a, complex_t b) {
    complex_t result = { 
        a.re * b.re - a.im * b.im,
        a.re * b.im + a.im * b.re
    };
    return result;
}

// Generate twiddle factors for FFT
void generate_twiddle_factors(complex_t* twiddle, int n) {
    for (int k = 0; k < n / 2; k++) {
        float angle = -2.0f * M_PI * k / n;
        twiddle[k].re = cosf(angle);
        twiddle[k].im = sinf(angle);
    }
}

// Bit-reverse an index for FFT
int bit_reverse(int x, int bits) {
    int result = 0;
    for (int i = 0; i < bits; i++) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

// Reorder array in bit-reversed order
void bit_reverse_array(complex_t* array, int n) {
    int bits = 0;
    int temp = n;
    while (temp > 1) {
        bits++;
        temp >>= 1;
    }
    
    for (int i = 0; i < n; i++) {
        int rev = bit_reverse(i, bits);
        if (i < rev) {
            complex_t tmp = array[i];
            array[i] = array[rev];
            array[rev] = tmp;
        }
    }
}

// Scalar implementation of Radix-2 FFT
void fft_scalar(complex_t* x, int n, complex_t* twiddle) {
    // Reorder input in bit-reversed order
    bit_reverse_array(x, n);
    
    // Perform FFT
    for (int stage = 1; stage <= log2f(n); stage++) {
        int m = 1 << stage;
        int m2 = m >> 1;
        
        for (int k = 0; k < n; k += m) {
            for (int j = 0; j < m2; j++) {
                complex_t t = complex_mul(x[k + j + m2], twiddle[j * n / m]);
                complex_t u = x[k + j];
                x[k + j] = complex_add(u, t);
                x[k + j + m2] = complex_sub(u, t);
            }
        }
    }
}

// NEON implementation of Radix-2 FFT
void fft_neon(complex_t* x, int n, complex_t* twiddle) {
    // Reorder input in bit-reversed order
    bit_reverse_array(x, n);
    
    // Perform FFT using NEON for butterfly operations
    for (int stage = 1; stage <= log2f(n); stage++) {
        int m = 1 << stage;
        int m2 = m >> 1;
        
        for (int k = 0; k < n; k += m) {
            for (int j = 0; j < m2; j += 2) {
                // Process two butterflies at once if possible
                if (j + 1 < m2) {
                    // Load twiddle factors
                    float32x4_t tw = vld1q_f32((float*)&twiddle[j * n / m]);
                    
                    // Load input values
                    float32x4_t a = vld1q_f32((float*)&x[k + j]);
                    float32x4_t b = vld1q_f32((float*)&x[k + j + m2]);
                    
                    // Perform complex multiplication using NEON
                    // (a+bi) * (c+di) = (ac-bd) + (ad+bc)i
                    float32x4_t tw_flip = vcombine_f32(
                        vext_f32(vget_low_f32(tw), vget_high_f32(tw), 1),
                        vext_f32(vget_low_f32(tw), vget_high_f32(tw), 1)
                    );
                    
                    // Negate imaginary part for complex multiplication
                    float32x4_t tw_conj = vreinterpretq_f32_u32(
                        veorq_u32(
                            vreinterpretq_u32_f32(tw_flip),
                            vreinterpretq_u32_f32(vdupq_n_f32(-0.0f))
                        )
                    );
                    
                    // Multiply real and swapped components
                    float32x4_t prod1 = vmulq_f32(b, tw);
                    float32x4_t prod2 = vmulq_f32(b, tw_conj);
                    
                    // Shuffle to get correct order
                    float32x4_t prod = vtrn1q_f32(prod1, prod2);
                    
                    // Compute butterfly
                    float32x4_t sum = vaddq_f32(a, prod);
                    float32x4_t diff = vsubq_f32(a, prod);
                    
                    // Store results
                    vst1q_f32((float*)&x[k + j], sum);
                    vst1q_f32((float*)&x[k + j + m2], diff);
                } else {
                    // Handle remaining butterfly (if odd number)
                    complex_t t = complex_mul(x[k + j + m2], twiddle[j * n / m]);
                    complex_t u = x[k + j];
                    x[k + j] = complex_add(u, t);
                    x[k + j + m2] = complex_sub(u, t);
                }
            }
        }
    }
}

// Generate a test signal (sum of sine waves)
void generate_test_signal(complex_t* signal, int n) {
    for (int i = 0; i < n; i++) {
        float t = (float)i / n;
        float val = 0.5f * sinf(2.0f * M_PI * 5.0f * t) +  // 5 Hz component
                    0.3f * sinf(2.0f * M_PI * 10.0f * t) + // 10 Hz component
                    0.1f * sinf(2.0f * M_PI * 20.0f * t);  // 20 Hz component
        signal[i].re = val;
        signal[i].im = 0.0f;
    }
}

// Print complex array (for debugging)
void print_complex_array(const complex_t* array, int n, const char* name) {
    printf("%s:\n", name);
    int print_count = (n > 16) ? 16 : n;  // Print at most 16 elements
    
    for (int i = 0; i < print_count; i++) {
        printf("[%2d] %8.4f%+8.4fi\n", i, array[i].re, array[i].im);
    }
    
    if (n > print_count) {
        printf("...\n");
    }
    printf("\n");
}

// Calculate magnitude of FFT output
void calculate_magnitude(const complex_t* fft, float* magnitude, int n) {
    for (int i = 0; i < n; i++) {
        magnitude[i] = sqrtf(fft[i].re * fft[i].re + fft[i].im * fft[i].im);
    }
}

// Verify two FFT results are approximately equal
bool verify_fft_results(const complex_t* a, const complex_t* b, int n, float epsilon) {
    for (int i = 0; i < n; i++) {
        float diff_re = fabsf(a[i].re - b[i].re);
        float diff_im = fabsf(a[i].im - b[i].im);
        
        if (diff_re > epsilon || diff_im > epsilon) {
            printf("Mismatch at index %d: (%f,%f) vs (%f,%f)\n",
                i, a[i].re, a[i].im, b[i].re, b[i].im);
            return false;
        }
    }
    
    return true;
}

int main(int argc, char** argv) {
    // Default FFT size (power of 2)
    int fft_size = 1024;
    
    // Allow overriding FFT size from command line
    if (argc > 1) {
        fft_size = atoi(argv[1]);
        
        // Ensure FFT size is a power of 2
        int log2_size = (int)log2f(fft_size);
        if (fft_size != (1 << log2_size)) {
            fprintf(stderr, "Error: FFT size must be a power of 2\n");
            return 1;
        }
    }
    
    printf("FFT Example\n");
    printf("-----------\n");
    printf("FFT Size: %d\n\n", fft_size);
    
    // Allocate memory for input/output
    complex_t* signal = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    complex_t* output_scalar = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    complex_t* output_neon = (complex_t*)neon_malloc(fft_size * sizeof(complex_t));
    float* magnitude = (float*)neon_malloc(fft_size * sizeof(float));
    complex_t* twiddle = (complex_t*)neon_malloc(fft_size/2 * sizeof(complex_t));
    
    if (!signal || !output_scalar || !output_neon || !magnitude || !twiddle) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    // Generate test signal
    generate_test_signal(signal, fft_size);
    
    // Generate twiddle factors
    generate_twiddle_factors(twiddle, fft_size);
    
    // Copy input signal to output buffers
    memcpy(output_scalar, signal, fft_size * sizeof(complex_t));
    memcpy(output_neon, signal, fft_size * sizeof(complex_t));
    
    // Create comparison timer
    perf_comparison_t* comp = comparison_create("FFT");
    
    // Run scalar FFT
    timer_start(comp->scalar_timer);
    fft_scalar(output_scalar, fft_size, twiddle);
    timer_stop(comp->scalar_timer);
    
    // Run NEON FFT
    timer_start(comp->simd_timer);
    fft_neon(output_neon, fft_size, twiddle);
    timer_stop(comp->simd_timer);
    
    // Verify results
    bool result_ok = verify_fft_results(output_scalar, output_neon, fft_size, 1e-3f);
    printf("Verification: %s\n", result_ok ? "PASSED" : "FAILED");
    
    // Calculate and print magnitude spectrum (first few frequencies)
    calculate_magnitude(output_neon, magnitude, fft_size);
    
    printf("Magnitude Spectrum (first 10 frequencies):\n");
    for (int i = 0; i < 10; i++) {
        float freq = (float)i * fft_size / fft_size;
        printf("[%2d] %8.2f Hz: %8.4f\n", i, freq, magnitude[i]);
    }
    printf("\n");
    
    // Print performance comparison
    comparison_print(comp);
    
    // Clean up
    free(signal);
    free(output_scalar);
    free(output_neon);
    free(magnitude);
    free(twiddle);
    comparison_destroy(comp);
    
    return result_ok ? 0 : 1;
}
