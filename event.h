#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

// ANSI Color codes - disabled
#define COLOR_RESET   ""
#define COLOR_RED     ""
#define COLOR_GREEN   ""
#define COLOR_YELLOW  ""
#define COLOR_BLUE    ""
#define COLOR_MAGENTA ""
#define COLOR_CYAN    ""
#define COLOR_WHITE   ""
#define COLOR_GRAY    ""

// Background colors - disabled
#define BG_RED        ""
#define BG_GREEN      ""
#define BG_YELLOW     ""
#define BG_BLUE       ""

// Symbols - disabled
#define SYMBOL_PRODUCER ""
#define SYMBOL_CONSUMER ""
#define SYMBOL_QUEUE    ""
#define SYMBOL_EVENT    ""
#define SYMBOL_CHECK    ""
#define SYMBOL_ARROW    ""
#define SYMBOL_WARNING  ""

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

