/**
 * Practice 3: Bully
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

/**
 * Function that sends the election message to the nodes greater than the current one (Election message=1)
 */
void send_Election_Message()
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
 * Function that receives the election messages from all its lower nodes (Election message=1).
 */
void recieve_Election_Message(int messages_election[])
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
 * Function that sends a (ok=2) or (timeout=3) message to all the nodes below the current one.
 * if the node is down.
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
 * Function that receives messages from nodes greater than the current node and stores their messages in an array.
 * Stores 2 if the node responds "ok".
 * Stores 3 if the node responds "timeout" * Stores 3 if the node responds "timeout" * For the lower nodes we store 0, 
 * representing no communication.
 * For the lower nodes we store 0, representing the lack of communication.
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
 * Function that sends the new leader message to all the nodes lower than the current one, after verifying that the 
 * current one is the highest ranking node complies with being the highest ranking.
 */
void send_Leader_Message(int *leader)
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
 * Function that receives leader messages from all nodes greater than the current one.
 * Stores zero in the locations corresponding to the nodes whose contact did not exist.
 */
void recieve_Leader_Message(int messages_l[])
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
 * Function that determines given an array of responses (ok, timeout and 0) if a node is the highest ranking node.
 * That is, if it receives only a "time out" message, and no "ok" message, then it is the highest ranking node
 * with the highest rank
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
 * Function that prints an array
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
    send_Election_Message();
    recieve_Election_Message(elections);
    send_Message_OK_Out(&fall);
    recieve_Messages_Ok_Out(oks_Or_Outs);
    // You can remove the documentation on these lines to see which messages each node receives respectively
    //[(0=no comunication), (2="ok"), (3="timeout")]
    int isL = isLeader(oks_Or_Outs, &fall);
    //printf("Messages received by the node (%d) are:\n", world_rank);
    //printArray(oks_Or_Outs);
    //printf("The node (%d) is the new leader? %s\n", world_rank, (isL == 1) ? "True" : "False");
    send_Leader_Message(&isL);
    recieve_Leader_Message(leader);
    for (int i = 0; i < world_size; ++i)
    {
        if (leader[i] == 4)
        {
            printf("(Node %d), my new boss is %d\n", world_rank, i);
            break;
        }
    }
    MPI_Finalize();
}