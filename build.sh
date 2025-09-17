#!/bin/bash
set -e

# Path to your NDK (adjust if installed somewhere else)
NDK_HOME="/home/ripper/Android/Sdk/ndk/ndk-28"

# Source file
SRC="fanotify_monitor.cpp"

# Output directory
OUT_DIR="build_out"
mkdir -p "$OUT_DIR"

# Android API level to target (21 = Lollipop, stable baseline)
API=21

# Architectures
ARCHS=("arm64-v8a" "armeabi-v7a" "x86" "x86_64")

# Extra include paths for Linux headers (to find sys/fanotify.h)
LINUX_INC="/usr/include"
LINUX_INC_MULTI="/usr/include/x86_64-linux-gnu"

echo "🔧 Building $SRC for Android with NDK ($NDK_HOME)..."

for ARCH in "${ARCHS[@]}"; do
    case $ARCH in
        "arm64-v8a")
            TARGET="aarch64-linux-android${API}"
            TOOL="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}-clang++"
            ;;
        "armeabi-v7a")
            TARGET="armv7a-linux-androideabi${API}"
            TOOL="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}-clang++"
            ;;
        "x86")
            TARGET="i686-linux-android${API}"
            TOOL="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}-clang++"
            ;;
        "x86_64")
            TARGET="x86_64-linux-android${API}"
            TOOL="$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}-clang++"
            ;;
    esac

    ARCH_OUT="$OUT_DIR/$ARCH"
    mkdir -p "$ARCH_OUT"

    echo "➡️ Building for $ARCH..."
    $TOOL -fPIC -shared "$SRC" \
        -llog \
        -o "$ARCH_OUT/libfanotify_monitor.so"

    echo "✅ Output: $ARCH_OUT/libfanotify_monitor.so"
done

echo "🎉 Build finished! Copy $OUT_DIR/* into your app's jniLibs/"

