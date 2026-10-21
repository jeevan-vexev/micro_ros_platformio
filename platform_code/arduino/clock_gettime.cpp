#include <Arduino.h>

#include <sys/time.h>
// micros() is a 32-bit count, so it wraps every 2^32 us exactly. One less leaves this clock
// a microsecond further behind millis() at every wrap, and from the second wrap (~143 min)
// the XRCE serial read loop can measure 0 ms for every 1 ms read and never time out.
#define micro_rollover_useconds 4294967296ULL

#if !defined(_POSIX_TIMERS) || !_POSIX_TIMERS

extern "C" int clock_gettime(clockid_t unused, struct timespec *tp)
{
    (void)unused;
    static uint32_t rollover = 0;
    static uint32_t last_measure = 0;

    uint32_t m = micros();
    rollover += (m < last_measure) ? 1 : 0;

    uint64_t real_us = (uint64_t) (m + rollover * micro_rollover_useconds);
    tp->tv_sec = real_us / 1000000;
    tp->tv_nsec = (real_us % 1000000) * 1000;
    last_measure = m;

    return 0;
}

#endif  // ifndef _POSIX_TIMERS
