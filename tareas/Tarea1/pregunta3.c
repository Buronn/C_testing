#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int channel[2];
int channel2[2];

char arr[10][10] =        //MENSAJES QUE ENVIARAN LOS THREADS
{ "h87tv5g2ic",
  "g1miscfnwr",
  "30wnhhaw90",
  "j1izl7iped",
  "w790e885i8",
  "naarxsdvvk",
  "f28x5ecz0g",
  "dcpp093a8f",
  "hotsesdios",
  "rrd2r0cng8"
};


struct arg_struct
{
    int id;
    int random;
};

void *escribir(void *xd)
{
    struct arg_struct *args = xd;
    srand(10*(int)args->random);
    int a = (int)(random () % 10);
    write(channel[1], &args->id, 1);
    write(channel2[1], arr[a], 9);
    printf("[%d]: %.*s\n", args->id,9, arr[a]);
    return NULL;
}
void *leer(void *arg)
{
    printf("Leyendo...\n");
    while (1)
    {
        sleep(1);
        int xd;
        char a[10];
        read(channel[0], &xd, 1);
        read(channel2[0],&a,9);
        printf("%d escribió %s\n",xd,a);
    }

    return NULL;
}


int main()
{
    pthread_t h2;
    pipe(channel);
    pipe(channel2);
    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        pthread_t h1;
        srand((int)h1);
        struct arg_struct args;
        args.id = i;
        args.random =random()%100;
        pthread_create(&h1, NULL, escribir, (void *)&args);
        
    }

    pthread_create(&h2, NULL, leer, NULL);
    pthread_join(h2, NULL);

    return 0;
}
