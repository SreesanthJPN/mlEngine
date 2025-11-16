#include <sys/resource.h>
#include <stdio.h>

int main() {
    struct rlimit limit;
    getrlimit(RLIMIT_NPROC, &limit);

    printf("Max tasks (processes + threads) for this user: %ld\n",
           (long)limit.rlim_cur);
}
