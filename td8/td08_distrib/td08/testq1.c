#include <stdio.h>

struct Data { // Une structure de données
    char nom[100];
    int age;
};

struct s2 {
    char c[3];
    int i;
    char c3;
};

int main(void) {
    struct Data d;
    struct s2 s;
    printf("%d\n", sizeof(d));
    printf("%d\n", sizeof(s));
}