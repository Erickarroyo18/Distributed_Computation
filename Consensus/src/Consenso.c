/**
 * Arroyo Martinez Erick Daniel
 * Computación Distribuida 2023-1 
 * Práctica 1: Consenso
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

/*
 *Funcion que asigna a los comandantes traidores
 */
void createTraitors(int nodes[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i % 3 == 1)
        {
            nodes[i] = 0;
        }
        else
        {
            nodes[i] = 1;
        }
    }
}
/*
Esta funcion envia la decision de un comandante dado que es o no un traidor
*/
void sendDecision(int *me, int *myDecision)
{
    srand(time(NULL) + world_rank);
    printf("El nodo %d, %s es Traidor\n", world_rank, *me ? "SI" : "NO");
    int vote;
    //Se genera asigna el voto original a los generales leales
    if(!*me){
        vote = *myDecision;
    }
    for (int i = 0; i < world_size; ++i)
    {
        if (i != world_rank)
        {
            if (*me)
            {
                int random = rand();
                vote = random % 2;
            }
            MPI_Send(&vote, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            
        }
    }
}
/*
 *Funcion que recibe la decision del resto de los comandantes
 */
void recieveDecisions(int decisions[], int *myDecision)
{
    for (int i = 0; i < world_size; ++i)
    {
        if (world_rank != i)
        {
            MPI_Recv(&decisions[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            decisions[i] = *myDecision;
        }
    }
}
/*
 *Funcion que envia el conjunto de decisiones recibidas por un comandante al resto de ellos
 * Esta funcion me dio errores, no se porque, por lo tanto lo implemente en el main
 */
void sendSetDecisions(int setDecisions[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i != world_rank)
        {
            MPI_Send(&setDecisions, world_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
}

/*
 *Funcion que recibe el conjunto de decisiones recibidas por cada comandante
 */
void recieveSetDecisions(int matriz[world_size][world_size], int fstDecisions[])
{
    for (int i = 0; i < world_size; ++i)
    {
        if (i != world_rank)
        {
            MPI_Recv(&matriz[i], world_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            for (int j = 0; j < world_size; ++j)
            {
                matriz[i][j] = fstDecisions[j];
            }
        }
    }
}

/*
 *Funcion que corrompe los mensajes obtenidos, si es que se trata de un traidor
 */
void corruptDecisions(int *me, int decisions[])
{
    if (*me)
    {
        srand(time(NULL) + world_rank);
        for (int i = 0; i < world_size; ++i)
        {
            int random = rand();
            decisions[i] = random % 2;
        }
    }
    else
    {
        return;
    }
}
/*
 *Funcion que dada la matriz de decisiones, recorre el conjunto de decisiones por cada
 */
void fstMajority(int matriz[world_size][world_size], int majority[])
{
    for (int i = 0; i < world_size; ++i)
    {
        int counterR = 0, counterA = 0;
        for (int j = 0; j < world_size; ++j)
        {
            int aux = matriz[j][i];
            if (aux == 1)
            {
                counterA++;
            }
            else if (aux == 0)
            {
                counterR++;
            }
        }
        if (counterR > counterA)
        {
            majority[i] = 0;
        }
        else if (counterR < counterA)
        {
            majority[i] = 1;
        }
        else
        {
            majority[i] = 0;
        }
    }
}
/**
 * Funcion que a partir de las mayorias de cada comandante, toma una decision
 **/
int finalDecision(int majorities[])
{
    int decision, counterA = 0, counterR = 0;
    for (int i = 0; i < world_size; i++)
    {
        int aux = majorities[i];
        if (aux == 0)
        {
            counterR++;
        }
        else if (aux == 1)
        {
            counterA++;
        }
    }
    if (counterR > counterA)
    {
        decision = 0;
    }
    else if (counterR < counterA)
    {
        decision = 1;
    }
    else
    {
        decision = 0;
    }
    return decision;
}

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int nodes[world_size]; // Arreglo que almanara el conjunto de comandantes
    int decisionByC[world_size];//Arreglo que almacena las decisiones de todos los comandantes
    int matrizDecisions[world_size][world_size];// Matriz que almacena el conjunto de arreglos de decisiones de cada comandante
    int fstMaj[world_size];//Arreglo que almacena las decisiones mayoritarias por comandante
    createTraitors(nodes); //Se deciden los comandantes leales y traidores
    int iAmTraitor = !nodes[world_rank];//Se verifica si el nodo es un traidor
    srand(time(NULL) + world_rank);
    int random = rand();
    int myDecision = random % 2;//Generamos decision inicial para cada general
    printf("La decision del nodo %d es (%d):%s \n", world_rank,myDecision, myDecision? "ATACAR" : "RETIRARSE");
    sendDecision(&iAmTraitor, &myDecision);//Se manda la decision del noto
    recieveDecisions(decisionByC, &myDecision);//Se reciben el resto de decisiones
    corruptDecisions(&iAmTraitor, decisionByC);//En caso de ser traidor, se corrompen las decisiones recibidas
    //Se envia el arreglo de decisiones a cada comandante
    for (int i = 0; i < world_size; ++i)
    {
        if (i == world_rank)
        {
            continue;
        }
        MPI_Send(&decisionByC, world_size, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    recieveSetDecisions(matrizDecisions, decisionByC);//Se reciben los arreglos de decisiones
    fstMajority(matrizDecisions, fstMaj);//Se calcula el arreglo de decisiones mayoritarias
    int finalD = finalDecision(fstMaj);//Se determina la desicion
    if (finalD)
    {
        printf("El comandante %d, ha decidido atacar\n", world_rank);
    }
    else
    {
        printf("El comandante %d, ha decidido retirarse\n", world_rank);
    }
    MPI_Finalize();
}