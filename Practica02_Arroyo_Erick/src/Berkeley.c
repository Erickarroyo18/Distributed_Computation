/**
 * Arroyo Martinez Erick Daniel
 * Computación Distribuida 2023-1
 * Práctica 2 : Berkeley
 * Profesor: Luis Germán Pérez Hernández
 * Ayudantes: Daniel y Fernando Michel Tavera
 **/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int world_rank;
int world_size;
/**
 * Funcion que genera un numero decimal aleatorio entre un valor minimo y maximo
 */
float randTime(float min, float max)
{
    srand(time(NULL) + world_rank);
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);       /* [min, max] */
}
/**
 * Funcion usada por el nodo maestro para enviar a cada nodo esclavo la solicitud de su tiempo local,
 * y que, a su vez, recibe el tiempo solicitado, calculando el retraso entre la hora de solicitud y recepcion
 * del tiempo
*/
void queryAndRecieve(float slavesTimes[], float *myTime)
{
    int message = 1;
    for (int i = 0; i < world_size; ++i)
    {
        if (i != 0)
        {
            int hourQuery = (int)time(NULL);
            MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Recv(&slavesTimes[i], 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int hourRecieve = (int)time(NULL);
            float delay = ((hourRecieve - hourQuery) / 2);
            printf("Nodo(%d), *RETRASO CALCULADO [%f]*\n", i,delay);
            slavesTimes[i] -= delay;
        }
        else
        {
            slavesTimes[i] = *myTime;
        }
    }
}

/**
 * Funcion que recive la petición del maestro solicitando el tiempo local de un nodo esclavo
 */
void recieveAsk(int *message)
{
    MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
/**
 * Funcion que calcula el promedio entres los tiempos de todos los esclavos y del maestro
 */
void avgTimes(float slaveTimes[], float *avg)
{
    for (int i = 0; i < world_size; ++i)
    {
        *avg += slaveTimes[i];
    }
    *avg = *avg / world_size;
}
/**
 * Funcion usada por el nodo maestro para mandarle a cada nodo esclavo 
 * su diferencia con respecto al promedio de los tiempos
 */
void sendTimeDiference(float *avg, float timeSlaves[])
{
    for (int i = 0; i < world_size; i++)
    {
        if (i != 0)
        {
            float diference = (*avg - timeSlaves[i]);
            MPI_Send(&diference, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            printf("*DIFERENCIA DEL NODO(%d) RESPECTO AL PROMEDIO [%f]*\n", i, diference);
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    float myTime = randTime(9.5, 10.5);
    if (world_rank == 0)
    {
        printf("Nodo(MAESTRO) *TIEMPO LOCAL [%f]*\n", myTime);
        float timesSlaves[world_size];
        queryAndRecieve(timesSlaves, &myTime);
        float avg;
        avgTimes(timesSlaves, &avg);
        printf("**PROMEDIO DE TIEMPOS[%f]**\n", avg);
        sendTimeDiference(&avg, timesSlaves);
        myTime += (avg - myTime);
        printf("Nodo(MAESTRO) *TIEMPO SINCRONIZADO [%f]*\n", myTime);
    }
    else
    {
        int message;
        float diference;
        recieveAsk(&message);
        MPI_Send(&myTime, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        printf("Nodo(%d) *TIEMPO LOCAL [%f]*\n", world_rank, myTime);
        MPI_Recv(&diference, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        myTime += diference;
        printf("Nodo(%d) *TIEMPO SINCRONIZADO [%f]*\n", world_rank, myTime);
    }

    MPI_Finalize();
}