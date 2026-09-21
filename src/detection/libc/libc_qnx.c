#include "libc.h"
#include "common/strutil.h"
#include "common/FFstrbuf.h"

const char* ffDetectLibc(FFLibcResult* result) {
    result->name = "libc";
    // libc version follows an exact to kernel version
    result->version = instance.state.platform.sysinfo.release.chars;
    return nullptr;
}
