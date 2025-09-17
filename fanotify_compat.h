#ifndef FANOTIFY_COMPAT_H
#define FANOTIFY_COMPAT_H

#include <linux/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========= Classic fanotify event masks ========= */
#ifndef FAN_ACCESS
#define FAN_ACCESS        0x00000001  /* File was accessed */
#endif
#ifndef FAN_MODIFY
#define FAN_MODIFY        0x00000002  /* File was modified */
#endif
#ifndef FAN_CLOSE_WRITE
#define FAN_CLOSE_WRITE   0x00000008  /* Writable file closed */
#endif
#ifndef FAN_CLOSE_NOWRITE
#define FAN_CLOSE_NOWRITE 0x00000010  /* Unwritable file closed */
#endif
#ifndef FAN_OPEN
#define FAN_OPEN          0x00000020  /* File was opened */
#endif
#ifndef FAN_Q_OVERFLOW
#define FAN_Q_OVERFLOW    0x00004000  /* Event queue overflowed */
#endif
#ifndef FAN_OPEN_PERM
#define FAN_OPEN_PERM     0x00010000  /* File open in perm check */
#endif
#ifndef FAN_ACCESS_PERM
#define FAN_ACCESS_PERM   0x00020000  /* File accessed in perm check */
#endif
#ifndef FAN_ONDIR
#define FAN_ONDIR         0x40000000  /* Event occurred against dir */
#endif
#ifndef FAN_EVENT_ON_CHILD
#define FAN_EVENT_ON_CHILD 0x08000000 /* Interested in child events */
#endif
#ifndef FAN_CLOSE
#define FAN_CLOSE         (FAN_CLOSE_WRITE | FAN_CLOSE_NOWRITE)
#endif

/* ========= Extended fanotify events (Linux ≥ 5.9) ========= */
#ifndef FAN_CREATE
#define FAN_CREATE        0x00000100  /* File was created */
#endif
#ifndef FAN_DELETE
#define FAN_DELETE        0x00000200  /* File was deleted */
#endif
#ifndef FAN_MOVED_FROM
#define FAN_MOVED_FROM    0x00000040  /* File moved from X */
#endif
#ifndef FAN_MOVED_TO
#define FAN_MOVED_TO      0x00000080  /* File moved to Y */
#endif
#ifndef FAN_MOVE
#define FAN_MOVE          (FAN_MOVED_FROM | FAN_MOVED_TO)
#endif

/* ========= fanotify_init flags ========= */
#ifndef FAN_CLOEXEC
#define FAN_CLOEXEC       0x00000001
#endif
#ifndef FAN_NONBLOCK
#define FAN_NONBLOCK      0x00000002
#endif
#ifndef FAN_CLASS_NOTIF
#define FAN_CLASS_NOTIF   0x00000000
#endif
#ifndef FAN_CLASS_CONTENT
#define FAN_CLASS_CONTENT 0x00000004
#endif
#ifndef FAN_CLASS_PRE_CONTENT
#define FAN_CLASS_PRE_CONTENT 0x00000008
#endif
#ifndef FAN_UNLIMITED_QUEUE
#define FAN_UNLIMITED_QUEUE 0x00000010
#endif
#ifndef FAN_UNLIMITED_MARKS
#define FAN_UNLIMITED_MARKS 0x00000020
#endif

/* ========= fanotify_mark flags ========= */
#ifndef FAN_MARK_ADD
#define FAN_MARK_ADD      0x00000001
#endif
#ifndef FAN_MARK_REMOVE
#define FAN_MARK_REMOVE   0x00000002
#endif
#ifndef FAN_MARK_DONT_FOLLOW
#define FAN_MARK_DONT_FOLLOW 0x00000004
#endif
#ifndef FAN_MARK_ONLYDIR
#define FAN_MARK_ONLYDIR  0x00000008
#endif
#ifndef FAN_MARK_MOUNT
#define FAN_MARK_MOUNT    0x00000010
#endif
#ifndef FAN_MARK_IGNORED_MASK
#define FAN_MARK_IGNORED_MASK 0x00000020
#endif
#ifndef FAN_MARK_IGNORED_SURV_MODIFY
#define FAN_MARK_IGNORED_SURV_MODIFY 0x00000040
#endif
#ifndef FAN_MARK_FLUSH
#define FAN_MARK_FLUSH    0x00000080
#endif


/* ========= Combined masks ========= */

#ifndef FAN_ALL_EVENTS
#define FAN_ALL_EVENTS (FAN_ACCESS | FAN_MODIFY | FAN_CLOSE | FAN_OPEN)
#endif

#ifndef FAN_ALL_PERM_EVENTS
#define FAN_ALL_PERM_EVENTS (FAN_OPEN_PERM | FAN_ACCESS_PERM)
#endif

#ifndef FAN_ALL_OUTGOING_EVENTS
#define FAN_ALL_OUTGOING_EVENTS (FAN_ALL_EVENTS | FAN_ALL_PERM_EVENTS | FAN_Q_OVERFLOW)
#endif

/* ========= Metadata ========= */
#ifndef FANOTIFY_METADATA_VERSION
#define FANOTIFY_METADATA_VERSION 3
#endif

struct fanotify_event_metadata {
    __u32 event_len;
    __u8 vers;
    __u8 reserved;
    __u16 metadata_len;
    __aligned_u64 mask;
    __s32 fd;
    __s32 pid;
};

struct fanotify_response {
    __s32 fd;
    __u32 response;
};

#ifndef FAN_ALLOW
#define FAN_ALLOW 0x01
#endif
#ifndef FAN_DENY
#define FAN_DENY  0x02
#endif
#ifndef FAN_NOFD
#define FAN_NOFD  -1
#endif

/* ========= Helpers ========= */
#ifndef FAN_EVENT_METADATA_LEN
#define FAN_EVENT_METADATA_LEN (sizeof(struct fanotify_event_metadata))
#endif

#ifndef FAN_EVENT_NEXT
#define FAN_EVENT_NEXT(meta, len) ((len) -= (meta)->event_len, \
    (struct fanotify_event_metadata*)(((char *)(meta)) + (meta)->event_len))
#endif

#ifndef FAN_EVENT_OK
#define FAN_EVENT_OK(meta, len)  ((long)(len) >= (long)FAN_EVENT_METADATA_LEN && \
    (long)(meta)->event_len >= (long)FAN_EVENT_METADATA_LEN && \
    (long)(meta)->event_len <= (long)(len))
#endif

/* ========= Syscall numbers ========= */
#if defined(__aarch64__)
  #define __SYS_fanotify_init 300
  #define __SYS_fanotify_mark 301
#elif defined(__arm__)
  #define __SYS_fanotify_init 367
  #define __SYS_fanotify_mark 368
#elif defined(__x86_64__)
  #define __SYS_fanotify_init 262
  #define __SYS_fanotify_mark 263
#elif defined(__i386__)
  #define __SYS_fanotify_init 338
  #define __SYS_fanotify_mark 339
#else
  #error "Unsupported architecture for fanotify syscalls"
#endif

/* ========= Inline wrappers ========= */
static inline int fanotify_init(unsigned int flags, unsigned int event_f_flags) {
    return syscall(__SYS_fanotify_init, flags, event_f_flags);
}

static inline int fanotify_mark(int fanotify_fd, unsigned int flags,
                                __u64 mask, int dirfd, const char *pathname) {
    return syscall(__SYS_fanotify_mark, fanotify_fd, flags, mask, dirfd, pathname);
}

#ifdef __cplusplus
}
#endif

#endif /* FANOTIFY_COMPAT_H */

