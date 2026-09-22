#include "cpucache.h"
#include <stdio.h>
#include <sys/syspage.h>

void recurseGetCache(FFCPUCacheResult* result, uint32_t idx, uint32_t prevLevel) {
    if (idx == CACHE_LIST_END) return;
    struct cacheattr_entry *cacheattr = SYSPAGE_ENTRY(cacheattr);

    FFCPUCacheType cacheType = FF_CPU_CACHE_TYPE_UNIFIED;
    if (cacheattr->flags & CACHE_FLAG_INSTR) {
        cacheType = FF_CPU_CACHE_TYPE_INSTRUCTION;
    } else if (cacheattr->flags & CACHE_FLAG_DATA) {
        cacheType = FF_CPU_CACHE_TYPE_DATA;
    } else if (cacheattr->flags & CACHE_FLAG_UNIFIED) {
        cacheType = FF_CPU_CACHE_TYPE_UNIFIED;
    }

    ffCPUCacheAddItem(result, prevLevel + 1, cacheattr->line_size * cacheattr->num_lines, cacheattr->line_size, cacheType);

    recurseGetCache(result, cacheattr->next, prevLevel + 1);
}

// Unfortunately the only cache api on QNX is self-declared, so it's not
// garanteed to be filled in, complete or accurate, however I will display it
// as it is provided
const char* ffDetectCPUCache(FFCPUCacheResult* result) {
    struct cpuinfo_entry *cpuinfo = SYSPAGE_ENTRY(cpuinfo);

    for (int i=0; i < _syspage_ptr->num_cpu; i++) {
        recurseGetCache(result, cpuinfo[i].ins_cache, 0);
        recurseGetCache(result, cpuinfo[i].data_cache, 0);
    }

    return nullptr;
}

