/**
 * platform_specific.h
 * Platform-specific code and optimizations for ARM NEON
 */
#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

#include "platform_detect.h"

/**
 * Platform-specific optimization hints and intrinsics
 */

#ifdef PLATFORM_MACOS
    #include <TargetConditionals.h>
    
    #ifdef PLATFORM_APPLE_SILICON
        // Apple Silicon specific optimizations
        #define PREFETCH_FOR_READ(addr) __builtin_prefetch(addr, 0, 3)
        #define PREFETCH_FOR_WRITE(addr) __builtin_prefetch(addr, 1, 3)
        
        // Apple Silicon has larger L1/L2 caches
        #define CACHE_LINE_SIZE 128
        
        // Apple Silicon has very fast vector units
        #define USE_ADVANCED_NEON_OPTIMIZATIONS 1
    #else
        // Generic ARM64 prefetch
        #define PREFETCH_FOR_READ(addr) __builtin_prefetch(addr, 0, 3)
        #define PREFETCH_FOR_WRITE(addr) __builtin_prefetch(addr, 1, 3)
        #define CACHE_LINE_SIZE 64
        #define USE_ADVANCED_NEON_OPTIMIZATIONS 0
    #endif
    
    // Darwin-specific includes
    #include <libkern/OSCacheControl.h>
    
    // Cache flush operation
    #define FLUSH_CACHE(addr, size) sys_icache_invalidate(addr, size)
    
#elif defined(PLATFORM_LINUX)
    // Generic Linux ARM64 settings
    #define PREFETCH_FOR_READ(addr) __builtin_prefetch(addr, 0, 3)
    #define PREFETCH_FOR_WRITE(addr) __builtin_prefetch(addr, 1, 3)
    #define CACHE_LINE_SIZE 64
    
    #ifdef PLATFORM_ANDROID
        // Android-specific settings
        #include <android/log.h>
        #define PLATFORM_LOG(...) __android_log_print(ANDROID_LOG_INFO, "NeonExplorer", __VA_ARGS__)
    #else
        #define PLATFORM_LOG(...) printf(__VA_ARGS__)
    #endif
    
    #ifdef PLATFORM_RPI
        // Raspberry Pi specific settings
        #if RPI_MODEL == 4
            // Raspberry Pi 4 has Cortex-A72
            #define CACHE_LINE_SIZE 64
            #define USE_ADVANCED_NEON_OPTIMIZATIONS 1
        #else
            // Older Raspberry Pi models
            #define CACHE_LINE_SIZE 32
            #define USE_ADVANCED_NEON_OPTIMIZATIONS 0
        #endif
    #endif
    
    // Linux ARM cache flush (requires privilege)
    static inline void flush_cache(void* addr, size_t size) {
        // On Linux, we typically rely on the kernel for cache coherence
        // This is just a placeholder - actual implementation might vary
        __builtin___clear_cache(addr, (char*)addr + size);
    }
    
    #define FLUSH_CACHE(addr, size) flush_cache(addr, size)

#elif defined(PLATFORM_FREEBSD)
    // FreeBSD ARM64 settings
    #define PREFETCH_FOR_READ(addr) __builtin_prefetch(addr, 0, 3)
    #define PREFETCH_FOR_WRITE(addr) __builtin_prefetch(addr, 1, 3)
    #define CACHE_LINE_SIZE 64
    #define USE_ADVANCED_NEON_OPTIMIZATIONS 0
    
    // FreeBSD ARM cache flush
    #include <machine/sysarch.h>
    
    static inline void flush_cache(void* addr, size_t size) {
        // FreeBSD-specific cache flush operation
        // This is just a placeholder - actual implementation might vary
        __builtin___clear_cache(addr, (char*)addr + size);
    }
    
    #define FLUSH_CACHE(addr, size) flush_cache(addr, size)
#else
    // Default settings for unknown platforms
    #define PREFETCH_FOR_READ(addr) __builtin_prefetch(addr, 0, 0)
    #define PREFETCH_FOR_WRITE(addr) __builtin_prefetch(addr, 1, 0)
    #define CACHE_LINE_SIZE 64
    #define USE_ADVANCED_NEON_OPTIMIZATIONS 0
    #define FLUSH_CACHE(addr, size) ((void)0)
    #define PLATFORM_LOG(...) printf(__VA_ARGS__)
#endif

/**
 * Platform-specific memory allocation
 */
static inline void* platform_malloc_aligned(size_t size, size_t alignment) {
    // Handle platform differences in aligned memory allocation
    void* ptr = NULL;
    
#if defined(PLATFORM_MACOS)
    // macOS - use posix_memalign
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
#elif defined(PLATFORM_ANDROID)
    // Android - use memalign from malloc.h
    #include <malloc.h>
    ptr = memalign(alignment, size);
#else
    // Standard - use posix_memalign
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
#endif
    
    return ptr;
}

/**
 * Platform-specific optimized functions
 */

#ifdef USE_ADVANCED_NEON_OPTIMIZATIONS
// Include any platform-specific optimized implementations here
#endif

#endif /* PLATFORM_SPECIFIC_H */
