#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>



int main(){
    int opcion;
    char *const arg[]={NULL};
    printf("||Menu||\n");
    printf("Escoga su opcion:\n");
    printf("(1) Canal 1\n");
    printf("(2) Canal 2\n");
    printf("(3) Canal 3\n");
    printf("(4) Salir\n");
    scanf("%d", &opcion);
    
    switch (opcion)
    {
    case 1:
        execv("./canal1",arg);
        break;

    case 2:
        execv("./canal2",arg);
        break;

    case 3:
        execv("./canal3",arg);
        break;
    
    case 4:
        exit(0);
        break;

    default:
        break;
    }

    return 0;
}