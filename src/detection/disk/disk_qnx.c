#include "disk.h"
#include "common/strutil.h"
#include <sys/statvfs.h>
#include <limits.h>

const char* ffDetectDisksImpl(FFDiskOptions* options, FFlist* disks) {
    // Disk APIs are is semi documented, so it's simpler to parse the output of
    // mount to get all disks, then statvfs to get block info
    FILE *fd = popen("mount", "r");

    if (fd == NULL) {
        return "Failed to run mount command";
    }

    // disk name and mount path could both be PATH_MAX, so giving some extra
    // room for the extra info
    char buff[PATH_MAX * 3] = {0};
    char disk[PATH_MAX], mountPath[PATH_MAX] = {0};
    char diskType[128] = {0};
    while ((fgets(buff, sizeof(buff), fd)) != NULL) {
        sscanf(buff, "%s on %s type %s", disk, mountPath, diskType);

        if (__builtin_expect(options->folders.length, 0)) {
            if (!ffStrbufSeparatedContainS(&options->folders, mountPath, FF_DISK_FOLDER_SEPARATOR)) {
                continue;
            }
        }

        if (options->hideFolders.length && ffDiskMatchesFolderPatterns(&options->hideFolders, mountPath, FF_DISK_FOLDER_SEPARATOR)) {
            continue;
        }

        if (options->hideFS.length && ffStrbufSeparatedContainS(&options->hideFS, diskType, ':')) {
            continue;
        }

        struct statvfs vfs = {0};
        if (statvfs(mountPath, &vfs) != 0) continue;

        FFDisk *new_disk = FF_LIST_ADD(FFDisk, *disks);
        ffStrbufAppendS(&new_disk->mountFrom, disk);
        ffStrbufAppendS(&new_disk->mountpoint, mountPath);
        ffStrbufAppendS(&new_disk->filesystem, diskType);
        new_disk->bytesTotal = vfs.f_blocks * vfs.f_bsize;
        new_disk->bytesAvailable = vfs.f_bavail * vfs.f_bsize;
        new_disk->bytesFree = vfs.f_bfree * vfs.f_bsize;
        new_disk->bytesUsed = new_disk->bytesTotal - new_disk->bytesFree;
        new_disk->filesUsed = (uint32_t) vfs.f_files;
    }

    pclose(fd);

    return nullptr;

}

