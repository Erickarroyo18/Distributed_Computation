# Distributed Computation (`MPI Message Exchange Algorithms`)
This repository documents the work conducted during the Distributed Computing course, focusing on the implementation of message exchange algorithms using the Message Passing Interface (MPI) in the C programming language. The repository comprises three practical assignments, each exploring different aspects of distributed computing.
## Contents
### Consensus Algorithm
In the `Consensus\src` folder, you'll find the implementation. The purpose of the program is to represent the behavior of the two-stage consensus algorithm. The program behaves as follows: The user indicates the number of processors (nodes) to be executed, from which the program determines that one third of the processors will be considered traitors (whose behavior indicates the algorithm's approach). In a first instance the program determines the loyalty of each commander to then send its decision to the rest of the commanders. Clarifying, the loyal commanders send the same decision to the rest of the commanders, and the traitors send different decisions. This for the first stage. For the second stage, the behavior is similar, i.e., the loyal commanders send the decisions of the rest of the commanders, as they received them, and the traitors corrupt those decision lists. To determine the majority decision, each commander determines the decision of the rest of them, from the information given in the arrangements.
### Berkeley Algorithm
In the `Berkeley\src` folder, you'll find the implementation.
* `MPI initialization`:
MPI is initialized and the current node rank (`world_rank`) and communicator size (`world_size`) are obtained.
* `Random Time Generation`:
The randTime function generates a random time between the given values (9.5 and 10.5) for each node. The seed is adjusted according to the node range to ensure different random numbers.
* `Query and Receive Times`:
The queryAndRecieve function is used by the master node to send time requests to the slave nodes, receive the requested times, and calculate the delay between request and receipt.
* `Average Time Calculation`:
The avgTimes function calculates the average time between the times of all nodes, including the master.
* `Sending Time Differences`:
The sendTimeDifference function is used by the master node to send the difference between its local time and the average time to each slave node.
* `Time Synchronization`:
At the master node, the master's local time is adjusted based on the average difference between the slave times and the average time.
* `MPI Communication`:
MPI functions such as MPI_Send and MPI_Recv are used for inter-node communication. Messages have a simple format, and nodes communicate with each other to request and share times.
* `Printout of Results`:
Relevant information is printed, such as the initial local time, the average time, the difference of each node with respect to the average, and the final synchronized time
### Bully Algorithm
In the `Bully\src` folder, you'll find the implementation. This code implements a bully algorithm in a distributed processing environment.
* `MPI initialization`:
MPI is initialized and the current node rank (`world_rank`) and communicator size (`world_size`) are obtained.
#### `Communication Functions`:
*   `send_Election_Message`: Sends election messages to nodes with ranks higher than the current node.
* `recieve_Election_Message`: Receives election messages from nodes with ranks lower than the current node.
* `send_Message_OK_Out`: Sends "_ok_" (2) or "_timeout_" (3) messages to nodes with ranks lower than the current node, indicating if the current node is down.
* `recieve_Messages_Ok_Out`: Receives "_ok_" or "_timeout_" messages from nodes with ranks higher than the current node.
#### `Leadership Functions`:
* `send_Leader_Message`: Sends leader messages to nodes with ranks lower than the current node.
* `recieve_Leader_Message`: Receives leadership messages from nodes with ranks higher than the current node.
* `isLeader`: Determines whether a node is the leader based on "_ok_" and "timeout" messages received.
#### Main Logic:
Initializes the elections, `oks_Or_Outs`, and leader arrays to store election messages, "_ok_" or "_timeout_" messages, and leadership messages, respectively.
The current node is checked to see if it is the last node in the range (fail is set to 1 if it is the last node).
Election messages and "_ok_" or "_timeout_" messages are sent and received. It is determined whether the current node is the leader using the isLeader function. `Leader messages are sent and received. The new leader is printed.
### Prerequisites
To test the provided implementations, the following prerequisites are needed:
* `MPI (Message Passing Interface)`:
MPI must be installed on the system. It can be installed using the operating system's package manager or downloaded from the official MPI site.
* `C compiler`:
A C compiler, such as GCC, must be installed on the system to compile MPI programs.
* `Bash environment`:
A Bash terminal or a compatible command line environment is required to run MPI programs.
### Running Algorithms 
* Clone this repository to your local machine:
```
git clone https://github.com/Erickarroyo18/Distributed_Computation.git
```
* Navigate to the specific algorithm folder:
```
cd Distributed_Computation/Algorithm_Name/src
```
Now compile the main file from a bash environment with
```
 mpicc -o executable_name Algorithm.c
```
For example (the name of the executable can be any, but it is recommended to use the same name of the algorithm):
```
 mpicc -o berkeley Berkeley.c
```
To run, use:
```
mpirun -np #nodes –oversubscribe ./Algorithm
```
For example:
```
 mpirun -np 3 –oversubscribe ./Berkeley
```
## Contributions
Contributions are welcome! If you find errors, have improvements, or new ideas, feel free to open an issue or submit a pull request.
## Contact
If you have any questions or comments, do not hesitate to contact me at [erickarroyo@ciencias.unam.mx](erickarroyo@ciencias.unam.mx).
Thanks for visiting this repository! I hope you find the content useful and educational