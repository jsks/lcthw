#include <stdio.h>
#include <string.h>

typedef struct {
    char *name;
    int age;
    int height;
    int weight;
} Person;

void Person_print(Person who) {
    printf("Name: %s\n", who.name);
    printf("Age: %d\n", who.age);
    printf("Height: %d\n", who.height);
    printf("Weight: %d\n", who.weight);
}

int main(void) {
    // Let's initialise our struct
    Person joe;
    joe.name = strdup("Joe Alex");
    joe.age = 32;
    joe.height = 64;
    joe.weight = 140;

    Person_print(joe);

    return 0;
}
