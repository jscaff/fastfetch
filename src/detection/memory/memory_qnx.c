#include "memory.h"
#include <sys/asinfo.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>

int countRam(struct asinfo_entry* as, [[maybe_unused]] char* name, void* ram) {
    *(unsigned long *)ram += as->end - as->start;
    return 1;
}

const char* ffDetectMemory(FFMemoryResult* ram) {
    walk_asinfo("ram", &countRam, &ram->bytesTotal);

    int fd = open("/proc/vm/stats", O_RDONLY);
    if (fd == -1) {
        return nullptr;
    }

    // Give absurdly large buffer because it truncates if there isn't enough space,
    // and you can't read multiple times
    char vmstat[4096];
    if (read(fd, vmstat, sizeof(vmstat)) < 0) {
        goto exit;
    }

    char *lineptr = vmstat;
    while (lineptr != NULL) {
        char *equalsIdx = strchr(lineptr, '=');
        if (equalsIdx == nullptr) break;

        uint64_t pageCount = 0;
        if (sscanf(lineptr, "pages_allocated=%lx ", &pageCount) > 0) {
            long pageSize = sysconf(_SC_PAGESIZE);
            if (pageSize < 0) pageSize = 4096;
            ram->bytesUsed = pageCount * (uint64_t) pageSize;
            break;
        }

        // to get the next line
        lineptr = strchr(lineptr, '\n') + 1;
    }

exit:
    close(fd);
    return nullptr;
}
