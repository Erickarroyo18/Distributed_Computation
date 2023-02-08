/**
 * Arroyo Martinez Erick Daniel
 * Computación Distribuida 2023-1
 * Práctica 1: Bully
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
 * Funcion que envia el mensaje de eleccion a los nodos mayores al actual (Mensaje de eleccion=1)
 */
void send_Message_Election()
{
    int election = 1;
    for (int i = 0; i < world_size; ++i)
    {
        if (i > world_rank)
        {
            MPI_Send(&election, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
}
/**
 * Funcion que recibe los mensajes de eleccion de todos sus nodos inferiores (Mensaje de eleccion=1)
 */
void recieve_Message_Election(int messages_election[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i < world_rank)
        {
            MPI_Recv(&messages_election[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}
/**
 * Funcion que envia a todos los nodos inferiores al actual un mensaje de (ok=2) o de (timeout=3)
 * si se trata de un nodo caido
 */
void send_Message_OK_Out(int *fall)
{
    int message = (*fall == 1) ? 3 : 2;
    for (int i = 0; i < world_size; ++i)
    {
        if (i < world_rank)
        {
            MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
}
/**
 * Funcion que recibe los mensajes de los nodos mayores al actual y almacena sus mensajes en un array
 * Guarda 2 si el nodo responde "ok"
 * Guarda 3 si el nodo responde "timeout"
 * Para los nodos inferiores guardamos 0, representando la falta de comunicacion
 */
void recieve_Messages_Ok_Out(int messages[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i > world_rank)
        {
            MPI_Recv(&messages[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            messages[i] = 0;
        }
    }
}
/**
 * Funcion que envia el mensaje de nuevo lider a todos los nodos inferiores al actual, tras verificar que el actual
 * cumpla con ser el de mayor rango
 */
void send_Message_Leader(int *leader)
{
    int message = (*leader == 1) ? 4 : 5;
    for (int i = 0; i < world_size; ++i)
    {
        if (i < world_rank)
        {
            MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
}
/**
 * Funcion que recibe los mensajes de lider de todos los nodos mayores al actual
 * Guarda cero en las localidades correspondiendes a los nodos cuyo contacto no existio
 */
void recieve_Messages_Leader(int messages_l[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i > world_rank)
        {
            MPI_Recv(&messages_l[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            messages_l[i] = 0;
        }
    }
}
/**
 * Funcion que determina dado un arreglo de respuestas (ok, timeout y 0) si un nodo es el de mayor rango
 * Es decir, si solo recibe un mensaje de "time out", y no recibe ningun mensaje de "ok", entonces este es el nodo
 * de mayor rango
 */
int isLeader(int messages[], int *fall)
{
    int leader = 0, counter = 0;
    for (int i = 0; i < world_size; ++i)
    {
        if (messages[i] == 2)
            counter++;
    }
    return (counter == 0 && *fall == 0);
}
/**
 * Funcion que imprime un array
 */
void printArray(int array[])
{
    printf("[");
    for (int i = 0; i < (world_size); ++i)
    {
        printf("%d, ", array[i]);
    }
    printf("]\n");
}
int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int elections[world_size];
    int oks_Or_Outs[world_size];
    int leader[world_size];
    int fall = (world_rank==world_size-1) ? 1 : 0;
    send_Message_Election();
    recieve_Message_Election(elections);
    send_Message_OK_Out(&fall);
    recieve_Messages_Ok_Out(oks_Or_Outs);
    // Puede quitarle la documentacion a estas lineas para ver que mensajes recibe cada nodo respectivamente
    //[(0=no comunicacion), (2="ok"), (3="timeout")]
    int isL = isLeader(oks_Or_Outs, &fall);
    //printf("Los mensajes recibidos por el nodo (%d) son:\n", world_rank);
    //printArray(oks_Or_Outs);
    //printf("El nodo (%d) es el nuevo lider? %s\n", world_rank, (isL == 1) ? "True" : "False");
    send_Message_Leader(&isL);
    recieve_Messages_Leader(leader);
    for (int i = 0; i < world_size; ++i)
    {
        if (leader[i] == 4)
        {
            printf("(Nodo %d), mi nuevo lider es %d\n", world_rank, i);
            break;
        }
    }
    MPI_Finalize();
}