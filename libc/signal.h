#ifndef __SIGNAL_H
#define __SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sighandler_t)(int);
#define SIG_ERR ((sighandler_t)-1)
#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
    unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

struct sigaction
{
    sighandler_t sa_handler;

    __sigset_t sa_mask;
    int sa_flags;
    void (*sa_restore) (void);
};

sighandler_t signal(int signum, sighandler_t handler);

#ifdef __cplusplus
}
#endif

#endif
