/*
 * Tijmen Zwaan - 10208917
 * Jurre Wolsing -
 *
 * Sieve of Eratosthenes
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "sieve.h"
#include "buffer.h"
#include "timer.h"

int BUFSIZE;
/* Used to temporarly store the thread_id of each thread. Not used currently */
pthread_t thread_id;

int main( int argc, char **argv ) {
    /* argument handling with defaults */
    if (argc < 2)
        BUFSIZE = 10;
    else
        BUFSIZE = atoi(argv[1]);

    /* Create buffer to next filter, and create filter thread */
    Buffer* next_buffer = make_buffer( BUFSIZE );
    pthread_create( &thread_id,
                    NULL,
                    &Filter,
                    next_buffer
    );

    /* Push numbers to the next filter */
    for (int current = 2;;current++)
        push(next_buffer, current);
}

void *Filter(void* arg) {
    int value,
        prime;
    Buffer *prev_buffer = (Buffer*) arg;
    Buffer *next_buffer = NULL;

    /* pop the actual prime number assigned to this thread */
    prime = pop(prev_buffer);
    printf("%d\n", prime);
    fflush(stdout);

    while(1) {
        value = pop(prev_buffer);

        /* Check if the next number is devisable by this prime number. If so
         * push it to the next thread */
        if (value % prime) {
            if (!next_buffer) {
                next_buffer = make_buffer( BUFSIZE );
                pthread_create( &thread_id,
                                NULL,
                                &Filter,
                                next_buffer
                );
            }
            push(next_buffer, value);
        }
    }
    return NULL;
}
