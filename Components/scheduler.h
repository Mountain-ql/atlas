/*
 * Copyright (C) 2006-2012 Michael Roitzsch <mroi@os.inf.tu-dresden.de>
 * economic rights: Technische Universitaet Dresden (Germany)
 *
 * Copyright (C) 2012 Stefan Wächtler
 */

#pragma mark ATLAS scheduler syscalls

#ifdef __linux__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#if defined(__x86_64__)
#define SYS_atlas_next   313
#define SYS_atlas_submit 314
#define SYS_atlas_debug  315
#elif defined(__i386__)
#define SYS_atlas_next   350
#define SYS_atlas_submit 351
#define SYS_atlas_debug  352
#else
#error Architecture not supported.
#endif

static inline int sched_submit(pid_t pid, struct timeval *exectime, struct timeval *deadline)
{
	if (syscall(SYS_atlas_submit, pid, exectime, deadline) == 0)
		return 0;
	else
		return errno;
}

inline int sched_next(void)
{
	return syscall(SYS_atlas_next);
}

inline int sched_debug(void)
{
	return syscall(SYS_atlas_debug);
}

#else

#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#warning Jobs will not be forwarded to the scheduler.

static inline int sched_submit(pid_t pid, struct timeval *exectime, struct timeval *deadline)
{
	return ENOTSUP;
}

inline int sched_next(void)
{
	return ENOTSUP;
}

inline int sched_debug(void)
{
	return ENOTSUP;
}

#endif

#pragma mark -


#pragma mark Compatibility Functions

static inline pid_t gettid(void)
{
#ifdef __linux__
	return syscall(SYS_gettid);
#else
	return 0;
#endif
}

#ifdef __APPLE__

#include <libkern/OSAtomic.h>

typedef OSSpinLock pthread_spinlock_t;

static inline int pthread_spin_init(pthread_spinlock_t *lock, int shared)
{
	*lock = OS_SPINLOCK_INIT;
	return 0;
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock)
{
	OSSpinLockLock(lock);
	return 0;
}

static inline int pthread_spin_unlock(pthread_spinlock_t *lock)
{
	OSSpinLockUnlock(lock);
	return 0;
}

static inline int pthread_spin_destroy(pthread_spinlock_t *lock)
{
	return 0;
}

#endif
