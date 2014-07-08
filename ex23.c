#include <stdio.h>
#include <string.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include "util/dbg.h"

#define DUFF(x, y) \
    case ((x) * 8 + (y) + 1): *to++ = *from++

#define SEVEN_DUFF(x) \
    DUFF(x, 6); \
    DUFF(x, 5); \
    DUFF(x, 4); \
    DUFF(x, 3); \
    DUFF(x, 2); \
    DUFF(x, 1); \
    DUFF(x, 0)

#define EIGHT_DUFF(x) \
    DUFF(x, 7); \
    SEVEN_DUFF(x)

struct TimerReturn {
    double elapsed;
    int rc;
};

typedef int (*FUNC_PTR)(char *from, char *to, int count);

int normal_copy(char *from, char *to, int count) {
    int i = 0;

    for (i = 0; i < count; i++) {
        to[i] = from[i];
    }

    return i;
}

int duffs_device(char *from, char *to, int count) {
    {
        int n = (count + 7) / 8;

        switch (count % 8) {
            case 0: do { *to++ = *from++;
                        case 7: *to++ = *from++;
                        case 6: *to++ = *from++;
                        case 5: *to++ = *from++;
                        case 4: *to++ = *from++;
                        case 3: *to++ = *from++;
                        case 2: *to++ = *from++;
                        case 1: *to++ = *from++;
                    } while (--n > 0);
        }
    }

    return count;
}

int duffs_device32(char *from, char *to, int count) {
    {
        int n = (count + 31) / 32;

        switch (count % 32) {
            case 0: do { *to++ = *from++;
                        SEVEN_DUFF(3);
                        EIGHT_DUFF(2);
                        EIGHT_DUFF(1);
                        EIGHT_DUFF(0);
                    } while (--n > 0);
        }
    }

    return count;
}

int zeds_device(char *from, char *to, int count) {
    {
        int n = (count + 7) / 8;

        switch (count % 8) {
            case 0:
            again: *to++ = *from++;

            case 7: *to++ = *from++;
            case 6: *to++ = *from++;
            case 5: *to++ = *from++;
            case 4: *to++ = *from++;
            case 3: *to++ = *from++;
            case 2: *to++ = *from++;
            case 1: *to++ = *from++;
                    if (--n > 0) goto again;
        }
    }

    return count;
}

int valid_copy(char *data, int count, char expects) {
    int i = 0;
    for (i = 0; i < count; i++) {
        if (data[i] != expects) {
            log_err("[%d] %c != %c", i, data[i], expects);
            return 0;
        }
    }

    return 1;
}

struct TimerReturn timer(FUNC_PTR func, char *from, char *to, int count) {
    struct TimerReturn tr;
    
    uint64_t begin = mach_absolute_time();
    tr.rc = func(from, to, count);
    uint64_t end = mach_absolute_time();

    tr.elapsed = (double)(end - begin) / 1000000000.0;
    return tr;
}


int main(int argc, char *argv[]) {
    char from[1000] = {'a'};
    char to[1000] = {'c'};
    struct TimerReturn tr;
    double sum = 0.0;

    // setup the `from` to have some stuff
    memset(from, 'x', 1000);

    // Normal copy 
    for (int i = 0; i < 100; i++) {
        memset(to, 'y', 1000);

        tr = timer(&normal_copy, from, to, 1000);
        check(tr.rc == 1000, "Normal copy failed: %d", tr.rc);
        check(valid_copy(to, 1000, 'x'), "Normal copy failed.");

        sum += tr.elapsed;
    }

    printf("Time elapsed for normal copy: %.9fs\n", sum / 100.0);

    // reset
    sum = 0.0;

    // Duff's version
    for (int i = 0; i < 100; i++) {
        memset(to, 'y', 1000);

        tr = timer(&duffs_device, from, to, 1000);
        check(tr.rc == 1000, "Duff's device failed: %d", tr.rc);
        check(valid_copy(to, 1000, 'x'), "Duff's device failed copy.");

        sum += tr.elapsed;
    }

    printf("Time elapsed for duffs_device: %.9fs\n", sum / 100.0);

    // reset
    sum = 0.0;

    // Zed's version
    for (int i = 0; i < 100; i++) {
        memset(to, 'y', 1000);

        tr = timer(&zeds_device, from, to, 1000);
        check(tr.rc == 1000, "Zed's device failed: %d", tr.rc);
        check(valid_copy(to, 1000, 'x'), "Zed's device failed copy.");

        sum += tr.elapsed;
    }

    printf("Time elapsed for zeds_device: %.9fs\n", sum / 100.0);

    return 0;

error:
    return 1;
}
