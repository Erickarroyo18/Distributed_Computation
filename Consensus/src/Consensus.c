/**
 * Practice 1: Consensus
 * Course: Distributed Computation
 * Author: Arroyo Martinez Erick Daniel 
 **/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int world_rank;
int world_size;

/*
 * Role assigned to traitor commanders
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
 * This function sends the decision of a given commander whether or not he is a traitor.
 */
void sendDecision(int *me, int *myDecision)
{
    srand(time(NULL) + world_rank);
    printf("Node %d, %s is Traitor\n", world_rank, *me ? "YES" : "NO");
    int vote;
    //The original vote is generated and assigned to loyal generals.
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
 * Function that receives the decision of the rest of the commanders.
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
 * Function that sends the set of decisions received by a commander to the rest of them.
 * This function gave me errors, I don't know why, so I implemented it in the main.
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
 * Function that receives the set of decisions received by each commander.
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
 * Function that corrupts the messages obtained, if it is a traitor.
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
 * Function that, given the decision matrix, runs through the set of decisions for each decision.
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

/*
 * Function that, based on the majorities of each commander, makes a decision
 */
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
    int nodes[world_size]; // Arrangement of the commanding officer's group
    int decisionByC[world_size];//Arrangement storing the decisions of all commanders
    int matrizDecisions[world_size][world_size];// Matrix storing the set of decision arrangements of each commander.
    int fstMaj[world_size];// Arrangement storing majority decisions by commander
    createTraitors(nodes); //Loyalist and traitor commanders are decided
    int iAmTraitor = !nodes[world_rank];//Checks if the node is a traitor
    srand(time(NULL) + world_rank);
    int random = rand();
    int myDecision = random % 2;//We generate initial decision for each general
    printf("La decision del nodo %d es (%d):%s \n", world_rank,myDecision, myDecision? "ATTACK" : "RETIRE");
    sendDecision(&iAmTraitor, &myDecision);//Node's decision is sent
    recieveDecisions(decisionByC, &myDecision);//The rest of the decisions are received
    corruptDecisions(&iAmTraitor, decisionByC);//In case of being a traitor, the decisions received will be corrupted.
    //The decision arrangement is sent to each commander.
    for (int i = 0; i < world_size; ++i)
    {
        if (i == world_rank)
        {
            continue;
        }
        MPI_Send(&decisionByC, world_size, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    recieveSetDecisions(matrizDecisions, decisionByC);//Decision arrangements are received
    fstMajority(matrizDecisions, fstMaj);//Majority decision arrangement is calculated
    int finalD = finalDecision(fstMaj);//The decision is determined
    if (finalD)
    {
        printf("Commander %d, has decided to attack\n", world_rank);
    }
    else
    {
        printf("Commander %d, has decided to retire\n", world_rank);
    }
    MPI_Finalize();
}