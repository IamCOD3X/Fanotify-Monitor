#include <jni.h>
#include <string>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "fanotify_compat.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <cstdio>
#include <cstring>

static JavaVM *gJvm = nullptr;
static jobject gCallbackObj = nullptr;
static jmethodID gOnEventMethod = nullptr;
static std::atomic<bool> gRunning(false);

void monitorLoop() {
    int fanFd = fanotify_init(FAN_CLASS_NOTIF | FAN_CLOEXEC, O_RDONLY | O_LARGEFILE);
    if (fanFd < 0) return;

    // Monitor the whole filesystem (root mount)
    if (fanotify_mark(fanFd, FAN_MARK_ADD | FAN_MARK_MOUNT, 
                      FAN_OPEN | FAN_ACCESS | FAN_MODIFY | FAN_CLOSE_WRITE | FAN_DELETE,
                      AT_FDCWD, "/")) {
        close(fanFd);
        return;
    }

    gRunning = true;

    while (gRunning) {
        struct fanotify_event_metadata buf[200];
        ssize_t len = read(fanFd, buf, sizeof(buf));
        if (len <= 0) continue;

        struct fanotify_event_metadata *meta;
        for (meta = buf; FAN_EVENT_OK(meta, len); meta = FAN_EVENT_NEXT(meta, len)) {
            if (meta->mask & FAN_Q_OVERFLOW) continue;

            char path[PATH_MAX] = {0};
            char linkPath[64];
            snprintf(linkPath, sizeof(linkPath), "/proc/self/fd/%d", meta->fd);
            readlink(linkPath, path, sizeof(path)-1);
            close(meta->fd);

            // Resolve process name from PID
            char procCmd[256] = {0};
            snprintf(linkPath, sizeof(linkPath), "/proc/%d/cmdline", meta->pid);
            FILE *f = fopen(linkPath, "r");
            if (f) {
                fgets(procCmd, sizeof(procCmd), f);
                fclose(f);
            }

            // Attach JVM and call back to Java
            JNIEnv *env;
            gJvm->AttachCurrentThread(&env, nullptr);

            jstring jPath = env->NewStringUTF(path);
            jstring jProc = env->NewStringUTF(procCmd);
            jlong jMask = (jlong)meta->mask;
            jint jPid = meta->pid;

            env->CallVoidMethod(gCallbackObj, gOnEventMethod, jPid, jProc, jPath, jMask);

            env->DeleteLocalRef(jPath);
            env->DeleteLocalRef(jProc);
        }
    }

    close(fanFd);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_iamcod3x_privacypeek_NativeFanotify_startMonitor(JNIEnv *env, jobject thiz) {
    if (gRunning) return;

    // Hold reference to Java callback
    gCallbackObj = env->NewGlobalRef(thiz);
    jclass clazz = env->GetObjectClass(thiz);
    gOnEventMethod = env->GetMethodID(clazz, "onFanotifyEvent",
                                      "(ILjava/lang/String;Ljava/lang/String;J)V");

    std::thread t(monitorLoop);
    t.detach();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_iamcod3x_privacypeek_NativeFanotify_stopMonitor(JNIEnv *, jobject) {
    gRunning = false;
}

jint JNI_OnLoad(JavaVM *vm, void *) {
    gJvm = vm;
    return JNI_VERSION_1_6;
}

