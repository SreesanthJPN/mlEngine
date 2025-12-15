#include <unistd.h>
#include <stdio.h>

long getL3Cache() {
    long l3_cache_size_bytes = sysconf(_SC_LEVEL3_CACHE_SIZE);
    if (l3_cache_size_bytes < 0) {
               perror("L3 cache size information not available via sysconf.\n");
    }
    return l3_cache_size_bytes;
}