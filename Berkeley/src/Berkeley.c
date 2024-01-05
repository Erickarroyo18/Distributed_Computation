/**
 * Practice 2: Berkeley
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
 * Function that generates a random decimal number between a minimum and a maximum value.
 */
float randTime(float min, float max)
{
    srand(time(NULL) + world_rank);
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);       /* [min, max] */
}

/**
 * Function used by the master node to send to each slave node the request for its local time,
 * and which, in turn, receives the requested time, calculating the delay between the time of request and reception
 * of the time
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
            printf("Node(%d), *ESTIMATED DELAY [%f]*\n", i,delay);
            slavesTimes[i] -= delay;
        }
        else
        {
            slavesTimes[i] = *myTime;
        }
    }
}

/**
 * Function that receives the request from the master requesting the local time of a slave node.
 */
void recieveAsk(int *message)
{
    MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/**
 * Function that calculates the average between the times of all slaves and the master.
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
 * Function used by the master node to send to each slave node 
 * its difference with respect to the average time.
 */
void sendTimeDiference(float *avg, float timeSlaves[])
{
    for (int i = 0; i < world_size; i++)
    {
        if (i != 0)
        {
            float diference = (*avg - timeSlaves[i]);
            MPI_Send(&diference, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            printf("*DIFFERENCE OF NODE(%d) FROM AVERAGE [%f]*\n", i, diference);
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
        printf("Node(MASTER) *LOCAL TIME [%f]*\n", myTime);
        float timesSlaves[world_size];
        queryAndRecieve(timesSlaves, &myTime);
        float avg;
        avgTimes(timesSlaves, &avg);
        printf("** TIMES AVERAGE [%f]**\n", avg);
        sendTimeDiference(&avg, timesSlaves);
        myTime += (avg - myTime);
        printf("Node(MASTER) *TIME SYNCHRONIZED [%f]*\n", myTime);
    }
    else
    {
        int message;
        float diference;
        recieveAsk(&message);
        MPI_Send(&myTime, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        printf("Node(%d) *LOCAL TIME [%f]*\n", world_rank, myTime);
        MPI_Recv(&diference, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        myTime += diference;
        printf("Node(%d) *TIME SYNCHRONIZED [%f]*\n", world_rank, myTime);
    }

    MPI_Finalize();
}