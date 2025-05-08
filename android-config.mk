# Android-specific Makefile configuration for ARM NEON

# Android NDK path (update this to your NDK path)
ANDROID_NDK_PATH ?= $(HOME)/Android/Sdk/ndk/25.1.8937393

# Android ABI and platform level
ANDROID_ABI ?= arm64-v8a
ANDROID_PLATFORM ?= android-33

# Android toolchain
ANDROID_TOOLCHAIN = $(ANDROID_NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64
ANDROID_AR = $(ANDROID_TOOLCHAIN)/bin/llvm-ar
ANDROID_CC = $(ANDROID_TOOLCHAIN)/bin/aarch64-linux-android$(ANDROID_PLATFORM_LEVEL)-clang
ANDROID_STRIP = $(ANDROID_TOOLCHAIN)/bin/llvm-strip

# Convert android-XX to just the number
ANDROID_PLATFORM_LEVEL = $(patsubst android-%,%,$(ANDROID_PLATFORM))

# Android-specific compiler flags
ANDROID_CFLAGS = -fPIC -D_ANDROID_ -DANDROID

# Android-specific libraries
ANDROID_LIBS = -lm -landroid -llog

# Android build targets
android-all: android-setup android-build

android-setup:
	@echo "Setting up Android build environment..."
	@if [ ! -d "$(ANDROID_NDK_PATH)" ]; then \
		echo "Error: Android NDK not found at $(ANDROID_NDK_PATH)"; \
		echo "Update ANDROID_NDK_PATH in android-config.mk"; \
		exit 1; \
	fi

android-build:
	@echo "Building for Android ($(ANDROID_ABI))..."
	@mkdir -p build/android/$(ANDROID_ABI)
	$(ANDROID_CC) $(CFLAGS) $(ANDROID_CFLAGS) -march=armv8-a+simd -o build/android/$(ANDROID_ABI)/vector_add_example examples/vector_add_example.c src/simd_ops.c -I./include $(ANDROID_LIBS)
	$(ANDROID_CC) $(CFLAGS) $(ANDROID_CFLAGS) -march=armv8-a+simd -o build/android/$(ANDROID_ABI)/dot_product_example examples/dot_product_example.c src/simd_ops.c -I./include $(ANDROID_LIBS)
	$(ANDROID_STRIP) build/android/$(ANDROID_ABI)/*

# Android APK build (requires Android SDK and Gradle)
# This is just a skeleton - you would need a proper Android project structure
android-apk:
	@echo "Please use Android Studio or Gradle to build the final APK."
	@echo "The Android binaries are in build/android/$(ANDROID_ABI)/"

.PHONY: android-all android-setup android-build android-apk
