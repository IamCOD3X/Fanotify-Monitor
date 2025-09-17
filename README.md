# Android Fanotify Monitor (JNI)

A portable C++ wrapper for **Linux fanotify** that works on Android via NDK.
It lets you monitor **file access, modification, open/close, creation, deletion, and moves** directly in your app.

---

## Features
- JNI bridge: callback into Java with `pid`, `process name`, `file path`, and event mask.
- Compatible with **old and new kernels** (defines missing events like `FAN_DELETE`, `FAN_CREATE`).
- Works on Android **arm64-v8a, armeabi-v7a, x86, x86_64**.

---

## Build

### Requirements
- Android NDK r28+ installed
- Linux host (tested on Ubuntu)

---

### Compile
```bash
bash build.sh
```
###
- The compiled .so libraries will appear in build_out/.

- Usage in Android Studio

    1. Copy .so files into your project:
    ```
    app/src/main/jniLibs/arm64-v8a/libfanotify.so
    app/src/main/jniLibs/armeabi-v7a/libfanotify.so
    app/src/main/jniLibs/x86/libfanotify.so
    app/src/main/jniLibs/x86_64/libfanotify.so
    ```
    2. Load the library in Java/Kotlin:
    ```
    static {
        System.loadLibrary("fanotify");
    }
    ```

    3. Implement the callback in your Java class:
    ```
    public void onFanotifyEvent(int pid, String process, String path, long mask) {
        Log.d("Fanotify", "PID=" + pid + " PROC=" + process + " PATH=" + path + " MASK=" + mask);
    }
    ```

    4. Start monitoring:
    ```
    NativeFanotify nf = new NativeFanotify();
    nf.startMonitor();
    ```

    5. Stop when done:
    ```
    nf.stopMonitor();
    ```
---

## License

MIT – feel free to use and modify.


## 👨‍💻 Developer

This project is developed and maintained by:

 - Sourabh Panchal (@IamCOD3X)
---

## ⚠️ Disclaimer

This library uses fanotify, which requires root and kernel support.  
It may not work on all Android devices.

---
