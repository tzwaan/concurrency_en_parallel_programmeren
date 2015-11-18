/*
 * MPI Assignment 3_2
 *
 * Tijmen Zwaan - 10208917
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MESSAGESIZE 128
#define ROOTNUM 3
#define TAG 42

/* Modulo */
unsigned mod(int a, unsigned b) {
    return (a >= 0 ? a % b : b - (-a) % b);
}

/* Broadcast 'buffer' with size 'count' and datatype 'datatype' from 'root' to all
 * other nodes in a circular manner over communicator 'communicator'
 */
int MYMPI_Bcast ( void * buffer ,           /* INOUT : buffer address */
                  int count ,               /* IN : buffer size */
                  MPI_Datatype datatype ,   /* IN : datatype of entry */
                  int root ,                /* IN : root process ( sender ) */
                  MPI_Comm communicator )   /* IN : communicator */
{
    /* Get own rank and size */
    int rank, size;
    MPI_Comm_rank( communicator, &rank);
    MPI_Comm_size( communicator, &size);

    /* Calculate relative left, relative right and end ranks */
    int end = mod(root + (size / 2), size);
    int left = mod((rank - 1), size);
    int right = mod((rank + 1), size);

    /* Ready for receiving */
    MPI_Status status;

    if( rank == root) {
        /* Root sends to 2 neighbours */
        printf("[%d: Out: %d Out: %d]\n", rank, left, right);

        MPI_Send(buffer, count, datatype, left, TAG, communicator);
        MPI_Send(buffer, count, datatype, right, TAG, communicator);

    } else if( rank == end) {
        /* Last one only receives from left */
        printf("[%d: In: %d]\n", rank, left);

        MPI_Recv(buffer, count, datatype, left, TAG, communicator, &status);

    } else if( mod(rank - root, size) < (size / 2) ) {
        /* Receive from left send to right*/
        printf("[%d: In: %d Out: %d]\n", rank, left, right);

        if( (rank - root) % size < (size / 2)) {
            MPI_Recv(buffer, count, datatype, left, TAG, communicator, &status);
            MPI_Send(buffer, count, datatype, right, TAG, communicator);
        }

    } else if ( mod(rank - root, size) > (size / 2)) {
        if(left == end) { // Only in, because end only accepts from left (meeting point for both ways)
            printf("[%d: In: %d]\n", rank, right);
            MPI_Recv(buffer, count, datatype, right, TAG, communicator, &status);
        } else {
            /* Receive from right send to left */
            printf( "[%d: In: %d Out: %d]\n", rank, right, left);

            MPI_Recv( buffer, count, datatype, right, TAG, communicator, &status);
            MPI_Send( buffer, count, datatype, left, TAG, communicator);
        }
    }

    return MPI_SUCCESS;
}



int main( int argc, char *argv[])
{
    /* Initialise MPI with rank and size */
    int rank, size;
    MPI_Init( &argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    /* Initiate valid root */
    int root = ROOTNUM % size;

    /* Local data of MPI process */
    char data[MESSAGESIZE];

    /* If root, create message to send */
    if(root == rank) {
        strcpy( data, "Hello World");
        printf( "%d: SEND '%s'\n", rank, data );
    }

    /* Do communication (sending or receiving) */
    MYMPI_Bcast(data, MESSAGESIZE, MPI_INT, root, MPI_COMM_WORLD);

    /* If not root, print received message */
    if( root != rank) {
        printf( "%d: GOT '%s'\n", rank, data );
    }

    /* End MPI */
    MPI_Finalize();

    return 0;
}
