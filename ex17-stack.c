#include <stdio.h>
#include <stdlib.h>

struct Stack {
    int size;
    int index;
    int contents[];
};

struct Stack *init(int size) {
    struct Stack *s;
    s = malloc(sizeof(struct Stack) + sizeof(int) * size);
    if (!s) return NULL;

    s->size = size;
    s->index = 0;

    return s;
}

void push(struct Stack *s, int obj) {
    s->contents[s->index++] = obj;
}

void pop(struct Stack *s) {
    s->index--;
}

int peek(struct Stack *s) {
    return s->contents[s->index - 1];
}

void destroy_stack(struct Stack *s) {
    if (s) free(s);
}

int main(void) {
    int size = 6;
    struct Stack *s = init(size);
    if (!s) {
        printf("Memory Error.\n");
        exit(1);
    }

    push(s, 2);
    push(s, 3);
    push(s, 4);
    pop(s);
    pop(s);
    printf("%d\n", peek(s));

    destroy_stack(s);

    return 0;
}
