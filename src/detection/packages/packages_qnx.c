#include "packages.h"
#include "common/strutil.h"
#include "common/io.h"

// Simplified version of the packages_linux implementation
// which would've required a decent amount of fixing since we handle dirent differently

static uint32_t getNumStringsImpl(const char* filename, const char* needle) {
    FF_STRBUF_AUTO_DESTROY content = ffStrbufCreate();
    if (!ffReadFileBuffer(filename, &content)) {
        return 0;
    }

    uint32_t count = 0;
    char* iter = content.chars;
    size_t needleLength = strlen(needle);
    while ((iter = memmem(iter, content.length - (size_t) (iter - content.chars), needle, needleLength)) != nullptr) {
        ++count;
        iter += needleLength;
    }

    return count;
}

static uint32_t getNumStrings(FFstrbuf* baseDir, const char* filename, const char* needle, const char* packageId) {
    uint32_t baseDirLength = baseDir->length;
    ffStrbufAppendS(baseDir, filename);

    FF_STRBUF_AUTO_DESTROY cacheDir = ffStrbufCreate();
    FF_STRBUF_AUTO_DESTROY cacheContent = ffStrbufCreate();

    uint32_t num_elements;
    if (ffPackagesReadCache(&cacheDir, &cacheContent, baseDir->chars, packageId, &num_elements)) {
        ffStrbufSubstrBefore(baseDir, baseDirLength);
        return num_elements;
    }

    num_elements = getNumStringsImpl(baseDir->chars, needle);
    ffStrbufSubstrBefore(baseDir, baseDirLength);

    ffPackagesWriteCache(&cacheDir, &cacheContent, num_elements);

    return num_elements;
}

void ffDetectPackagesImpl(FFPackagesResult* result, FFPackagesOptions* options) {
    FF_STRBUF_AUTO_DESTROY baseDir = ffStrbufCreateA(512);
    ffStrbufAppendS(&baseDir, FASTFETCH_TARGET_DIR_ROOT);

    if (FF_PACKAGES_IS_ENABLED(options, APK)) {
        result->apk += getNumStrings(&baseDir, "/lib/apk/db/installed", "C:Q", "apk");
    }
}
