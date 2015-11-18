/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include "simulate.h"


/* Add any global variables you may need. */


/* Add any functions you may need (like a worker) here. */


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array)
{
    int rc, num_tasks, my_rank, left_neighbour, right_neighbour, i_this,
        i, j, t, start, end;
    MPI_Request left_out_handle;
    MPI_Request right_out_handle;
    MPI_Request left_in_handle;
    MPI_Request right_in_handle;
    double *this_old_array, *this_current_array, *this_next_array, *temp;

    rc = MPI_Init(NULL, NULL);

    if (rc != MPI_SUCCESS) {
        fprintf( stderr, "Unable to set up MPI");
        MPI_Abort( MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size( MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank);

    left_neighbour = my_rank-1;
    right_neighbour = my_rank+1;

    i_this = i_max / num_tasks;
    start = i_this * my_rank;
    end = start + i_this;

    this_old_array = calloc(sizeof(double), (i_this+2));
    this_current_array = calloc(sizeof(double), (i_this+2));
    this_next_array = calloc(sizeof(double), (i_this+2));

    if (my_rank > 0) {
        this_old_array[0] = old_array[start-1];
    }
    if (my_rank < num_tasks-1) {
        this_old_array[i_this+1] = old_array[end+1];
    }
    for (i=start, j=1; i<end; i++, j++) {
        this_old_array[j] = old_array[i];
        this_current_array[j] = current_array[i];
    }

    printf( "I'm task nr %d/%d\n", my_rank+1, num_tasks);
    fflush(stdout);

    for (t=0; t<t_max; t++) {
        if (my_rank > 0) {
            MPI_Isend(&this_current_array[1], 1, MPI_DOUBLE, left_neighbour, ((t+1)*2)-1,
                    MPI_COMM_WORLD, &left_out_handle);
            MPI_Irecv(&this_current_array[0], 1, MPI_DOUBLE, left_neighbour, ((t+1)*2)+1,
                    MPI_COMM_WORLD, &left_in_handle);
        }
        if (my_rank < num_tasks-1) {
            MPI_Isend(&this_current_array[i_this], 1, MPI_DOUBLE,
                    right_neighbour, ((t+1)*2)+1, MPI_COMM_WORLD, &right_out_handle);
            MPI_Irecv(&this_current_array[i_this+1], 1, MPI_DOUBLE,
                    right_neighbour, ((t+1)*2)-1, MPI_COMM_WORLD, &right_in_handle);
        }
        calc_row(i_this, this_old_array, this_current_array, this_next_array);

        if (my_rank > 0) {
            MPI_Wait(&left_out_handle, NULL);
            MPI_Wait(&left_in_handle, NULL);
        }
        if (my_rank < num_tasks-1) {
            MPI_Wait(&right_in_handle, NULL);
            MPI_Wait(&right_out_handle, NULL);
        }
        calc_edges(i_this, this_old_array, this_current_array, this_next_array);


        temp = this_old_array;
        this_old_array = this_current_array;
        this_current_array = this_next_array;
        this_next_array = temp;
    }

    for (i=start, j=1; i<end; i++, j++) {
        current_array[i] = this_current_array[j];
    }

    /* You should return a pointer to the array with the final results. */
    free(this_old_array);
    free(this_current_array);
    free(this_next_array);
    MPI_Finalize();
    return current_array;
}

void calc_row(int length, double *old, double *cur, double *new) {
    int i;
    double c = 0.15;
    for (i=2; i<length; i++) {
        new[i] = 2.0 * cur[i] - old[i] + c*(cur[i-1] - (2.0 * cur[i] - cur[i+1]));
    }
}
void calc_edges(int length, double *old, double *cur, double *new) {
    int i = 1;
    double c = 0.15;
    new[i] = 2.0 * cur[i] - old[i] + c*(cur[i-1] - (2.0 * cur[i] - cur[i+1]));
    i = length;
    new[i] = 2.0 * cur[i] - old[i] + c*(cur[i-1] - (2.0 * cur[i] - cur[i+1]));
}
