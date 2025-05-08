/**
 * platform_detect.h
 * Platform detection and feature identification for cross-platform ARM NEON development
 */
#ifndef PLATFORM_DETECT_H
#define PLATFORM_DETECT_H

/* Platform detection */
#if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS 1
    #define PLATFORM_NAME "macOS"
    #if defined(__arm64__) || defined(__aarch64__)
        #define PLATFORM_APPLE_SILICON 1
    #endif
#elif defined(__FreeBSD__)
    #define PLATFORM_FREEBSD 1
    #define PLATFORM_NAME "FreeBSD"
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
    #define PLATFORM_NAME "Linux"
    
    /* Detect specific Linux platforms */
    #if defined(__ANDROID__)
        #define PLATFORM_ANDROID 1
        #define PLATFORM_SPECIFIC "Android"
    #else
        /* Try to detect Raspberry Pi */
        #include <stdio.h>
        #include <stdlib.h>
        static inline int is_raspberry_pi(void) {
            FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
            if (!cpuinfo) return 0;
            
            char line[256];
            int is_pi = 0;
            
            while (fgets(line, sizeof(line), cpuinfo)) {
                if (strstr(line, "Raspberry Pi")) {
                    is_pi = 1;
                    break;
                }
            }
            
            fclose(cpuinfo);
            return is_pi;
        }
        
        #define PLATFORM_MAYBE_RPI (is_raspberry_pi())
    #endif
#else
    #warning "Unknown platform detected"
    #define PLATFORM_UNKNOWN 1
    #define PLATFORM_NAME "Unknown"
#endif

/* Architecture detection */
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    #define ARCH_ARM64 1
    #define ARCH_NAME "ARM64"
    
    /* NEON/SIMD feature detection */
    #if defined(__ARM_NEON)
        #define HAS_NEON 1
    #endif
    
    /* ARMv8.2-A with half-precision floating point */
    #if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
        #define HAS_NEON_FP16 1
    #endif
    
    /* ARMv8.2-A with dotprod instructions */
    #if defined(__ARM_FEATURE_DOTPROD)
        #define HAS_NEON_DOTPROD 1
    #endif
    
    /* Check for specific ISA versions */
    #if defined(__ARM_ARCH)
        #define ARM_ARCH_VERSION __ARM_ARCH
    #endif
#else
    #error "This project requires ARM64 architecture"
#endif

/* Compiler detection */
#if defined(__clang__)
    #define COMPILER_CLANG 1
    #define COMPILER_NAME "Clang"
    #define COMPILER_VERSION_MAJOR __clang_major__
    #define COMPILER_VERSION_MINOR __clang_minor__
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
    #define COMPILER_NAME "GCC"
    #define COMPILER_VERSION_MAJOR __GNUC__
    #define COMPILER_VERSION_MINOR __GNUC_MINOR__
#else
    #define COMPILER_UNKNOWN 1
    #define COMPILER_NAME "Unknown"
#endif

/* Helper function to print platform information */
static inline void print_platform_info(void) {
    printf("Platform: %s\n", PLATFORM_NAME);
    
#if defined(PLATFORM_MAYBE_RPI) && PLATFORM_MAYBE_RPI
    printf("Specific platform: Raspberry Pi\n");
#elif defined(PLATFORM_SPECIFIC)
    printf("Specific platform: %s\n", PLATFORM_SPECIFIC);
#endif
    
    printf("Architecture: %s\n", ARCH_NAME);
    
#if defined(ARM_ARCH_VERSION)
    printf("ARM Architecture Version: %d\n", ARM_ARCH_VERSION);
#endif
    
    printf("Compiler: %s %d.%d\n", 
           COMPILER_NAME, 
           COMPILER_VERSION_MAJOR, 
           COMPILER_VERSION_MINOR);
    
    /* Feature checks */
    printf("NEON Support: %s\n", 
#if defined(HAS_NEON)
           "Yes"
#else
           "No"
#endif
    );
    
    printf("FP16 Support: %s\n", 
#if defined(HAS_NEON_FP16)
           "Yes"
#else
           "No"
#endif
    );
    
    printf("DotProduct Support: %s\n", 
#if defined(HAS_NEON_DOTPROD)
           "Yes"
#else
           "No"
#endif
    );
}

#endif /* PLATFORM_DETECT_H */
