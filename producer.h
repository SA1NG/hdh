#ifndef PRODUCER_H
#define PRODUCER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "time.h"
#include "event.h"

#define MAX_PRODUCERS 10
#define MAX_CONSUMERS 10

extern int event_come_delay;
extern int num_producers;
extern int num_consumers;

// Structure for timed events in mode 3
typedef struct {
    int timestamp;      // When to create event (seconds)
    char event_char;    // Event character (s, t, a-z, q)
} TimedEvent;

typedef struct
{
    BlockingQueue* queue;
    int command_from_user;   // for what mode is used (1 or 2 or 3)
    char* string_from_user;  // for input string from user in mode 2
    TimedEvent* timed_events; // for mode 3 timed events
    int num_timed_events;    // number of timed events
    int thread_id;           // producer thread ID
    int total_producers;     // total number of producers
    int total_consumers;     // total number of consumers
} dataSendToProducer;

void* producer_run(void* arg);

#endif

