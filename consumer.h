#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "queue.h"

#define TIME_DELAY_SIMULATION 3 // 3s

typedef struct
{
    BlockingQueue* queue;
    int thread_id;
} dataSendToConsumer;

void array_key_init();
void* consumer_run(void* arg);

#endif

