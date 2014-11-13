#include <stdio.h>
#include <ctype.h>
#include "../util/dbg.h"

int print_msg(const char *msg) {
    printf("A STRING: %s\n", msg);

    return 0;
}

int uppercase(const char *msg, int msglen) {
    for (int i = 0; msg[i] < msglen; i++) {
        printf("%c", toupper(msg[i]));
    }

    printf("\n");
    return 0;
}

int lowercase(const char *msg, int msglen) {
    for (int i = 0; msg[i] < msglen; i++) {
        printf("%c", tolower(msg[i]));
    }

    printf("\n");

    return 0;
}

int fail_on_purpose(const char *msg) {
    return 1;
}
