#include "os.h"

void ffDetectOSImpl(FFOSResult* os) {
    ffStrbufSetStatic(&os->name, "QNX");
    ffStrbufSet(&os->version, &instance.state.platform.sysinfo.release);
}
