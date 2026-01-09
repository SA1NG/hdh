#include "event.h"

ButtonEvent button_event[26];
EventCount event_count = {0, 0, 0, 0};
pthread_mutex_t event_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t button_event_mutex = PTHREAD_MUTEX_INITIALIZER;

void enqueue_done(Event* e, int slots_have_event) 
{
    const char* type_str = "UNKNOWN";
    int num = 0;
    switch(e->type) {
        case EVENT_SENSOR: type_str = "SENSOR"; num = e->number.sensor_event; break;
        case EVENT_BUTTON: type_str = "BUTTON"; num = e->number.button_event; break;
        case EVENT_TIME: type_str = "TIME"; num = e->number.time_event; break;
        case EVENT_SHUTDOWN: type_str = "SHUTDOWN"; num = e->number.shutdown_event; break;
    }
    printf("    [QUEUE] ➜  Enqueue %-8s #%-3d | Queue: [%d/%d]\n", type_str, num, slots_have_event, QUEUE_CAPACITY);
}

void dequeue_done(Event* e, int slots_have_event, int consumer_id)
{
    const char* type_str = "UNKNOWN";
    int num = 0;
    switch(e->type) {
        case EVENT_SENSOR: type_str = "SENSOR"; num = e->number.sensor_event; break;
        case EVENT_BUTTON: type_str = "BUTTON"; num = e->number.button_event; break;
        case EVENT_TIME: type_str = "TIME"; num = e->number.time_event; break;
        case EVENT_SHUTDOWN: type_str = "SHUTDOWN"; num = e->number.shutdown_event; break;
    }
    printf("    [QUEUE] ←  Dequeue %-8s #%-3d | Queue: [%d/%d] | Consumer-%d\n", type_str, num, slots_have_event, QUEUE_CAPACITY, consumer_id);
}


