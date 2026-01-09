#include "consumer.h"

void array_key_init()
{
    for(int i=0; i<26; i++)
    {
        button_event[i].key = i + 97;
        button_event[i].count = 0;
    }
}

void handle_sensor(Event* e, int consumer_id) 
{
    printf("[Consumer %d] Process: SENSOR #%d Value: %d\n",
        consumer_id, e->number.sensor_event, e->data.sensor_value);
}

void handle_button(Event* e, int consumer_id) 
{
    pthread_mutex_lock(&button_event_mutex);
    for(int i=0; i<26; i++)
    {
        if(e->data.key == button_event[i].key)
        {
            button_event[i].count++;
            printf("[Consumer %d] Process: BUTTON #%d Key: '%c' (count: %d)\n",
                consumer_id, e->number.button_event, button_event[i].key, button_event[i].count);
            pthread_mutex_unlock(&button_event_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&button_event_mutex);
}

void handle_time(Event* e, int consumer_id) 
{
    printf("[Consumer %d] Process: TIME #%d %s\n",
        consumer_id, e->number.time_event, e->data.str);
    free(e->data.str);
}

void handle_event(Event* e, int consumer_id)
{
    sleep(e->processing_time);
    switch (e->type) 
    {
        case EVENT_SENSOR: handle_sensor(e, consumer_id); break;
        case EVENT_BUTTON: handle_button(e, consumer_id); break;
        case EVENT_TIME: handle_time(e, consumer_id); break;
        case EVENT_SHUTDOWN: break;
        default: break;
    }
}
void* consumer_run(void* arg) 
{
    dataSendToConsumer* data = (dataSendToConsumer*)arg;
    BlockingQueue* queue = data->queue;
    int thread_id = data->thread_id;
    
    printf("[Consumer %d] Started\n", thread_id);

    while (1) 
    {
        Event* e = (Event*)queue_dequeue(queue, thread_id);
        
        if (!e) 
        {
            printf("[Consumer %d] Queue stopped, exiting\n", thread_id);
            break;
        }

        if (e->type == EVENT_SHUTDOWN) 
        {
            printf("[Consumer %d] Received SHUTDOWN\n", thread_id);
            
            // Only first shutdown prints summary
            static int summary_printed = 0;
            pthread_mutex_lock(&button_event_mutex);
            if(!summary_printed) {
                summary_printed = 1;
                int total = event_count.sensor_event + event_count.button_event + 
                           event_count.time_event + event_count.shutdown_event;
                printf("\n========================================\n");
                printf("          EVENT SUMMARY\n");
                printf("========================================\n");
                printf("  Total Events:      %-4d\n", total);
                printf("----------------------------------------\n");
                printf("  SENSOR events:    %-4d\n", event_count.sensor_event);
                printf("  BUTTON events:    %-4d\n", event_count.button_event);
                if(event_count.button_event > 0) {
                    for(int i=0; i<26; i++) {
                        if(button_event[i].count!=0) 
                            printf("     '%c': %d times\n", button_event[i].key, button_event[i].count); 
                    }
                }
                printf("  TIME events:      %-4d\n", event_count.time_event);
                printf("  SHUTDOWN events:  %-4d\n", event_count.shutdown_event);
                printf("========================================\n\n");
            }
            pthread_mutex_unlock(&button_event_mutex);
            
            free(e);
            break;
        }
        
        handle_event(e, thread_id);
        free(e);
    }
    
    printf("[Consumer %d] Finished\n", thread_id);
    return NULL;
}

