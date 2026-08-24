#include <unistd.h>
#include <stdio.h>

#define L3_CACHE_FALLBACK_BYTES (8L * 1024 * 1024)

long getL3Cache() {
    long l3_cache_size_bytes = sysconf(_SC_LEVEL3_CACHE_SIZE);
    if (l3_cache_size_bytes <= 0) {
        fprintf(stderr, "L3 cache size not available via sysconf; falling back to 8MB.\n");
        l3_cache_size_bytes = L3_CACHE_FALLBACK_BYTES;
    }
    return l3_cache_size_bytes;
}