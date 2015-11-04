#include <stdlib.h>
#include <pthread.h>
#include "buffer.h"

/* pop a value from the buffer */
int pop( Buffer* buffer) {
    int value;
    if (!buffer) {
        return 0;
    }
    /* Lock buffer to avoid race conditions */
    pthread_mutex_lock( &buffer->buflock );
    /* If buffer is empty, wait thread until signal */
    if ( buffer->occupied == 0 ) {
        pthread_cond_wait( &buffer->bufsignal, &buffer->buflock );
    }

    /* pop value from buffer */
    value = buffer->buffer[buffer->nextOut];
    buffer->nextOut = (buffer->nextOut + 1) % buffer->size;
    buffer->occupied--;

    /* Send signal that theres room in the buffer */
    pthread_cond_signal( &buffer->bufsignal );

    pthread_mutex_unlock( &buffer->buflock );
    return value;
}

int push( Buffer* buffer, int value ) {
    if (!buffer) {
        return 0;
    }
    /* Lock buffer to avoid race conditions */
    pthread_mutex_lock( &buffer->buflock );
    /* If buffer is full, wait thread until signal */
    if ( buffer->occupied == buffer->size ) {
        pthread_cond_wait( &buffer->bufsignal, &buffer->buflock );
    }

    /* add value to the buffer */
    buffer->buffer[buffer->nextIn] = value;
    buffer->nextIn = (buffer->nextIn + 1) % buffer->size;
    buffer->occupied++;

    /* Send signal that theres a value in the buffer */
    if ( buffer->occupied > (buffer->size / 2)) {
        pthread_cond_signal( &buffer->bufsignal );
    }

    pthread_mutex_unlock( &buffer->buflock );
    return 0;
}

Buffer* make_buffer( int size ) {
    /* Intialisation of the buffer */
    Buffer* buffer = malloc( sizeof(Buffer) );
    buffer->buffer = malloc( size * sizeof(int) );
    buffer->size = size;
    buffer->occupied = 0;
    buffer->nextIn = 0;
    buffer->nextOut = 0;
    pthread_mutex_init( &buffer->buflock, NULL );
    pthread_cond_init( &buffer->bufsignal, NULL );

    return buffer;
}

void remove_buffer( Buffer* buffer ) {
    /* Free the buffer.
     * Not used since program never ends. */
    free( buffer->buffer );
    free( buffer );
}
