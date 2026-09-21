#include "cpu.h"
#include <sys/syspage.h>

const char* ffDetectCPUImpl(const FFCPUOptions* options, FFCPUResult* cpu) {
    // struct cpuinfo_entry *cpuinfo = _SYSPAGE_ENTRY(_syspage_ptr, cpuinfo);
    // cpu->name = SYSPAGE_ENTRY(strings)->data + cpuinfo->name;
    // cpu->coresPhysical = _syspage_ptr->num_cpu;
    // cpu->coresLogical = 0;

    return "Not supported on this platform";
}
