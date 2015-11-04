#ifndef BUFFER_H
#define BUFFER_H
#include <pthread.h>

typedef struct Buffer {
    int* buffer;
    int size,
        occupied,
        nextIn,
        nextOut;
    pthread_mutex_t buflock;
    pthread_cond_t bufsignal;
} Buffer;

int pop(Buffer*);
int push(Buffer*, int);
Buffer* make_buffer(int);
void remove_buffer(Buffer*);

#endif
