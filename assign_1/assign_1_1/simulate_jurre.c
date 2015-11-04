/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "simulate.h"

void *next_generation (void *args) {
    struct arg_struct *arguments = args;
    int from = arguments->from;
    int to = arguments->to;
    double *old_array = arguments->old_array;
    double *current_array = arguments->current_array;
    double *next_array = arguments->next_array;
    
    
    
    int i;
    for (i = from; i < to; i++) {
        next_array[i] = 2 * current_array[i] - old_array[i] + 
            PARAMETER_C * (current_array[i-1] - 2 * current_array[i] + current_array[i+1]);
    }
    return NULL;
}
    
    

double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    pthread_t thread_ids[num_threads];
    double *temp;
    
    int t;
    for (t = 0; t < t_max; t++) {
        int j;
        for (j = 0; j < num_threads ; j++) {
            int from, to;
            
            if (j == 0) {
                from = 1;
            }
            else {
                from = j * i_max / num_threads;
            }
            
            if (j == num_threads - 1) {
                to = i_max - 1;
            }
            else {
                to = (j+1) * i_max / num_threads;
            }
            
            struct arg_struct *arguments = malloc(sizeof(struct arg_struct));
            arguments->from = from;
            arguments->to = to;
            arguments->old_array = old_array;
            arguments->current_array = current_array;
            arguments->next_array = next_array;
            
            pthread_create(&thread_ids[j],
            NULL,
            &next_generation,
            (void *)arguments);
	}
            
            
    	for (int i = 0; i < num_threads; i ++) {
       	    pthread_join(thread_ids[i], NULL);
    	}     
        
        temp = old_array;
        old_array = current_array;
        current_array = next_array;
        next_array = temp;
    }
    
    
    return current_array;
}
