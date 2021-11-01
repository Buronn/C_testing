#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

int main()
{
    printf("Bienvenido al canal 1\n");
    fflush(stdout);
    sleep(3);
    execlp("./pregunta2", "", NULL);
    return 0;
}