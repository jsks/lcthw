#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dbg.h"

#define MAX_DATA 100

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

int str_input(char *str, size_t size) {
    char c;
    int n = 0;

    while (scanf("%c", &c) != 0) {
        if (n == size - 1 || c == '\n') break;
        if (isalnum(c) || c == '.') {
            str[n] = c;
            n++;
        }
    }

    str[n] = '\0';
    return n;
}

int main(int argc, char *argv[]) {
    Person you = { .age = 0 };
    char temp[MAX_DATA];

    printf("What's your First Name? ");
    str_input(you.first_name, MAX_DATA);
    check(strlen(you.first_name) != 0, "Failed to read first name.");

    printf("What's your Last Name? ");
    str_input(you.last_name, MAX_DATA);
    check(strlen(you.last_name) != 0, "Failed to read last name.");

    printf("How old are you? ");
    str_input(temp, MAX_DATA);
    check(strlen(temp) != 0, "Failed to read age.");
    you.age = atoi(temp);

    printf("What color are your eyes: \n");
    for (int i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i+1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");

    str_input(temp, MAX_DATA);
    check(strlen(temp) != 0, "Failed to read eyes");
    you.eyes = atoi(temp) - 1;

    check(you.eyes <= OTHER_EYES && you.eyes >= 0, "Do it right, that's not an option.");

    printf("How much do you make an hour? ");
    str_input(temp, MAX_DATA);
    check(strlen(temp) != 0, "Enter a floating point number.");
    you.income = strtof(temp, NULL);

    printf("-------- Results --------\n");

    printf("First Name: %s\n", you.first_name);
    printf("Last Name: %s\n", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %.2f\n", you.income);

    return 0;

error:
    return -1;
}
