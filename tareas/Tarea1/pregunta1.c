#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>


int main(void)
{
    float k;
    printf("Escriba el numero de procesos: \n");
    scanf("%f", &k); //Input de cantidad de procesos
    float a = 0;
    while (a != k)
    {
        int pid = fork(); //SE CREA HIJO
        if (pid != 0) //VERIFICA SI EL PROCESO ES PADRE
        {
            srand(time(NULL) ^ getpid()); //SEMILLA RANDOM SEGUN EL PID DEL PADRE
            int r = (rand() % (6)); //SLEEP RANDOM
            sleep(r);
        }
        else //SI NO ES PADRE HACE ESTO:
        {
            srand(time(NULL) ^ getpid()); //SEMILLA RANDOM SEGUND PID DEL HIJO
            int r = (rand() % (11)); //SLEEP RANDOM
            sleep(r); 

            if (getppid() != 1) //SI EL PROCESO HIJO TERMINA ANTES QUE EL PROCESO PADRE, SERÁ ZOMBIE
            {
                printf("PAPA VIVO \n");
                printf("ZOMBIE Proceso: %d, Padre: %d, Numero aleatorio: %d\n", getpid(), getppid(), r);
            }
            else //SI EL PROCESO HIJO TERMINA DESPUÉS QUE EL PROCESO PADRE, SERÁ ZOMBIE
            {
                printf("PAPA MUERTO \n");
                printf("HUERFANO Proceso: %d, Padre: %d, Numero aleatorio: %d\n", getpid(), getppid(), r);
            }

            exit(0);
        }
        a++; 
    }
    return 0;
}
