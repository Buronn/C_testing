#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    char a[2][14];

    strcpy(a[0], "blah");
    strcpy(a[1], "hmm");
    printf("mensaje: %s\n", &a[0]);
    return 0;
}