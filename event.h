#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include <pthread.h>
#include "queue.h"

typedef struct
{
    char key;
    int count;
} ButtonEvent;
extern ButtonEvent button_event[26];

typedef struct
{
    int sensor_event;
    int button_event;
    int time_event;
    int shutdown_event;
} EventCount;
extern EventCount event_count;

extern pthread_mutex_t event_count_mutex;
extern pthread_mutex_t button_event_mutex;

typedef enum 
{
    EVENT_SENSOR,
    EVENT_BUTTON,
    EVENT_TIME,
    EVENT_SHUTDOWN
} EventType;

typedef struct 
{
    char* str;
    char key;
    int sensor_value;
} EventData;

typedef struct 
{
    EventType type;
    EventData data;
    EventCount number;
    int processing_time;
} Event;

void enqueue_done(Event* e, int slots_have_event);
void dequeue_done(Event* e, int slots_have_event, int consumer_id);

#endif

