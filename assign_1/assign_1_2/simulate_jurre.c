/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */
#include <stdlib.h>
#include <stdio.h>

#include "omp.h"
#include "simulate.h"  
    

double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    double *temp_array;
    
    omp_set_num_threads(num_threads);
    
    for (int t = 0; t < t_max; t++) {
        # pragma omp parallel for
        for (int i = 1; i < i_max-1; i++) {
        next_array[i] = 2 * current_array[i] - old_array[i] + 
            PARAMETER_C * (current_array[i-1] - 2 * current_array[i] + current_array[i+1]);
        }
        
        temp_array = old_array;
        old_array = current_array;
        current_array = next_array;
        next_array = temp_array;
        
        /*for (int i = 1; i < i_max-1; i++) {
            printf("%f\t", current_array[i]);
        }
        printf("\n");*/
    }
    
    
    return current_array;
}
