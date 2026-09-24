#include "processes.h"
#include <sys/debug.h>
#include <sys/procfs.h>
#include <devctl.h>
#include <limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

const char* ffDetectProcesses(const FFProcessesOptions* options, FFProcessesResult* result) {
    DIR* dfd = opendir("/proc");
    if (dfd) {
        struct dirent* entry;
        char buf[PATH_MAX] = {0};
        int pid = 0;
        while ((entry = readdir(dfd)) != nullptr) {
            // Only match integer entries since they're the pids
            if (sscanf(entry->d_name, "%d", &pid) <= 0) continue;
            if (!options->countKprocs && pid == 1) { // Kernel Task
                continue;
            }

            snprintf(buf, sizeof(buf), "/proc/%d/ctl", pid);
            int ctlfd = open(buf, O_RDONLY);
            if (ctlfd == -1) continue;


            procfs_info info;
            if (devctl(ctlfd, DCMD_PROC_INFO, &info, sizeof(info), nullptr) == EOK) {
                result->processes++;
                result->threads += info.num_threads;
            }

            close(ctlfd);
        }
    }
    closedir(dfd);

    return nullptr;
}
