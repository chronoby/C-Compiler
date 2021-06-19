#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char str[100];
    scanf("%s",str);
    char test[100] = "./scanner ";
    char result[100];
    strcpy(result, test);
    strcat(result, str);
    system(result);
    return 0;
}
