# ARM NEON Intrinsics Cheat Sheet

## Common Data Types

- `int8x8_t`, `int8x16_t`: 8-bit signed integers (8 or 16 elements)
- `uint8x8_t`, `uint8x16_t`: 8-bit unsigned integers (8 or 16 elements)
- `int16x4_t`, `int16x8_t`: 16-bit signed integers (4 or 8 elements)
- `uint16x4_t`, `uint16x8_t`: 16-bit unsigned integers (4 or 8 elements)
- `int32x2_t`, `int32x4_t`: 32-bit signed integers (2 or 4 elements)
- `uint32x2_t`, `uint32x4_t`: 32-bit unsigned integers (2 or 4 elements)
- `int64x1_t`, `int64x2_t`: 64-bit signed integers (1 or 2 elements)
- `uint64x1_t`, `uint64x2_t`: 64-bit unsigned integers (1 or 2 elements)
- `float32x2_t`, `float32x4_t`: 32-bit floating point (2 or 4 elements)
- `float64x1_t`, `float64x2_t`: 64-bit floating point (1 or 2 elements)

## Naming Conventions

- Operations on 64-bit registers: `vop_type(args)`
- Operations on 128-bit registers: `vopq_type(args)`
- `type` indicates operand types:
  - `s8`, `s16`, `s32`, `s64`: Signed integers
  - `u8`, `u16`, `u32`, `u64`: Unsigned integers
  - `f32`, `f64`: Floating point
  - `p8`, `p16`: Polynomials

## Load Operations

```c
// Load single vector (64-bit)
int32x2_t v = vld1_s32(int32_t const * ptr);
// Load single vector (128-bit)
int32x4_t v = vld1q_s32(int32_t const * ptr);

// Load multiple vectors
uint8x8x2_t v = vld2_u8(uint8_t const * ptr); // Two interleaved vectors
uint8x8x3_t v = vld3_u8(uint8_t const * ptr); // Three interleaved vectors
uint8x8x4_t v = vld4_u8(uint8_t const * ptr); // Four interleaved vectors

// Accessing elements from multiple vectors
uint8_t val = v.val[0][3]; // First vector, fourth element
```

## Store Operations

```c
// Store single vector (64-bit)
vst1_s32(int32_t * ptr, int32x2_t val);
// Store single vector (128-bit)
vst1q_s32(int32_t * ptr, int32x4_t val);

// Store multiple vectors
vst2_u8(uint8_t * ptr, uint8x8x2_t val); // Two interleaved vectors
vst3_u8(uint8_t * ptr, uint8x8x3_t val); // Three interleaved vectors
vst4_u8(uint8_t * ptr, uint8x8x4_t val); // Four interleaved vectors
```

## Arithmetic Operations

```c
// Addition
int32x4_t sum = vaddq_s32(a, b);
// Subtraction
int32x4_t diff = vsubq_s32(a, b);
// Multiplication
int32x4_t prod = vmulq_s32(a, b);
// Division (no direct intrinsic, use reciprocal approximation for floats)
float32x4_t quot = vdivq_f32(a, b);
```

## Multiply-Accumulate Operations

```c
// a = a + (b * c)
int32x4_t res = vmlaq_s32(a, b, c);
// a = a - (b * c)
int32x4_t res = vmlsq_s32(a, b, c);
```

## Logic Operations

```c
// Bitwise AND
uint32x4_t res = vandq_u32(a, b);
// Bitwise OR
uint32x4_t res = vorrq_u32(a, b);
// Bitwise XOR
uint32x4_t res = veorq_u32(a, b);
// Bitwise NOT
uint32x4_t res = vmvnq_u32(a);
```

## Comparison Operations

```c
// Greater than
uint32x4_t res = vcgtq_s32(a, b);
// Greater than or equal
uint32x4_t res = vcgeq_s32(a, b);
// Equal
uint32x4_t res = vceqq_s32(a, b);
// Less than or equal
uint32x4_t res = vcleq_s32(a, b);
// Less than
uint32x4_t res = vcltq_s32(a, b);
```

## Data Movement

```c
// Duplicate a value to all lanes
int32x4_t v = vdupq_n_s32(10);
// Set all lanes to a specific array of values
int32x4_t v = vld1q_s32((int32_t[]){1, 2, 3, 4});
// Extract a single lane
int32_t val = vgetq_lane_s32(v, 2); // Get the 3rd element
// Set a single lane
int32x4_t new_v = vsetq_lane_s32(99, v, 1); // Set the 2nd element to 99
```

## Vector Creation

```c
// Create vector from individual elements
uint32x4_t v = vcombine_u32(vdup_n_u32(1), vdup_n_u32(2));
// Create constant vector
float32x4_t ones = vdupq_n_f32(1.0f); // [1.0, 1.0, 1.0, 1.0]
```

## Conversion Operations

```c
// Convert float to int (truncate)
int32x4_t i = vcvtq_s32_f32(f);
// Convert int to float
float32x4_t f = vcvtq_f32_s32(i);
// Signed to unsigned
uint32x4_t u = vreinterpretq_u32_s32(s);
```

## Math Functions

```c
// Absolute value
int32x4_t abs_v = vabsq_s32(v);
// Square root (approximation)
float32x4_t sqrt_v = vrsqrteq_f32(v);
// Min value
int32x4_t min_v = vminq_s32(a, b);
// Max value
int32x4_t max_v = vmaxq_s32(a, b);
```

## Permutation & Rearrangement

```c
// Interleave even elements
uint32x2x2_t res = vzip_u32(a, b);
// Interleave odd elements
uint32x2x2_t res = vuzp_u32(a, b);
// Transpose elements
uint32x2x2_t res = vtrn_u32(a, b);
// Extract subset of elements from two vectors
uint8x8_t res = vext_u8(a, b, 3); // Extract starting from 3rd element of a
```

## Reduction Operations

```c
// Horizontal add across vector (64-bit)
int32x2_t sum2 = vpadd_s32(a, a);
// Horizontal add across vector (128-bit)
int32x2_t sum2 = vaddq_s32(vget_low_s32(a), vget_high_s32(a));
// Add all elements together (fully reduced)
int32_t sum = vaddvq_s32(a); // ARMv8.1+
```

## Common Patterns & Idioms

### Horizontal Sum of Vector

```c
// Sum all elements in a 128-bit vector
float32x4_t v = ...; // Your vector
float32x2_t sum2 = vadd_f32(vget_low_f32(v), vget_high_f32(v));
float32x2_t sum1 = vpadd_f32(sum2, sum2);
float sum = vget_lane_f32(sum1, 0);
```

### Vector Magnitude (L2 Norm)

```c
float32x4_t v = ...; // Your vector
float32x4_t v_squared = vmulq_f32(v, v);
float32x2_t sum2 = vadd_f32(vget_low_f32(v_squared), vget_high_f32(v_squared));
float32x2_t sum1 = vpadd_f32(sum2, sum2);
float magnitude = sqrt(vget_lane_f32(sum1, 0));
```

### Vectorized Clamp

```c
float32x4_t clamp(float32x4_t x, float32x4_t min_v, float32x4_t max_v) {
    return vminq_f32(vmaxq_f32(x, min_v), max_v);
}
```
