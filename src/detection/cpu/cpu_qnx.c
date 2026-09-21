#include "cpu.h"
#include <sys/syspage.h>

const char* ffDetectCPUImpl(const FFCPUOptions* options, FFCPUResult* cpu) {
    struct cpuinfo_entry *cpuinfo = SYSPAGE_ENTRY(cpuinfo);
    ffStrbufAppendS(&cpu->name, SYSPAGE_ENTRY(strings)->data + cpuinfo->name);

    // Logical cores aren't real and QNX doesn't support hyperthreading
    cpu->coresOnline = cpu->coresLogical = cpu->coresPhysical = _syspage_ptr->num_cpu;

    // TODO implement aarch64 detection using cpuinfo->flags
    ffCPUDetectByCpuid(cpu);

    // Naive implementation assuming symmetric cores
    // A more granular breakdown can be implemented in the future
    cpu->frequencyBase = cpu->frequencyMax = cpu->coreTypes[0].freq = cpuinfo->speed;
    cpu->coreTypes[0].count = _syspage_ptr->num_cpu;

    return nullptr;
}
