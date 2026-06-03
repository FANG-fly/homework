#include <stdio.h>

int main() {
    char name[100];
    
    printf("Hello, World!\n");
    printf("Please enter your name: ");
    scanf("%s", name);
    printf("Hello, %s! Welcome!\n", name);
    
    return 0;
}
