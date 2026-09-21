#include "os.h"
#include <sys/utsname.h>

void ffDetectOSImpl(FFOSResult* os) {
    ffStrbufSetStatic(&os->name, "QNX");
    // The minor kernel version isn't important for the release name
    struct utsname uts;
    if (uname(&uts) == 0) {
        ffStrbufSetS(&os->version, uts.release);
    }else {
        ffStrbufSet(&os->version, &instance.state.platform.sysinfo.release);
    }
}
