#include "uptime.h"
#include "common/time.h"

#include <time.h>

const char* ffDetectUptime(FFUptimeResult* result) {
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
        return nullptr;

    result->uptime = (uint64_t)tp.tv_sec * 1000 + (uint64_t)((double) tp.tv_nsec / 1e6);
    result->bootTime = ffTimeGetNow() - result->uptime;
    return nullptr;
}
