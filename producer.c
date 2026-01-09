#include "producer.h"
#include "consumer.h"

void get_datetime(char *buf, size_t buf_size)
{
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", &tm_now);
}

int create_events(char c, Event* e, int producer_id)
{
    pthread_mutex_lock(&event_count_mutex);
    
    if (c == 's') 
    {
        event_count.sensor_event++;
        e->number.sensor_event = event_count.sensor_event;
        e->type = EVENT_SENSOR;
        e->data.sensor_value = rand() % 100;
        e->processing_time = 1;  // SENSOR: 1 second
        printf("[Producer-%d] ✓ Created SENSOR   #%-3d (Process: 1s)\n", producer_id, e->number.sensor_event);
    } 
    else if (c == 't') 
    {
        event_count.time_event++;
        e->number.time_event = event_count.time_event;
        e->type = EVENT_TIME;
        char* datetime_buf = malloc(32);
        get_datetime(datetime_buf, 32);
        e->data.str = datetime_buf;
        e->processing_time = 3;  // TIME: 3 seconds
        printf("[Producer-%d] ✓ Created TIME     #%-3d (Process: 3s)\n", producer_id, e->number.time_event);
    } 
    else if (c == 'q') 
    {
        event_count.shutdown_event++;
        e->number.shutdown_event = event_count.shutdown_event;
        e->type = EVENT_SHUTDOWN;
        e->processing_time = 0;
        pthread_mutex_unlock(&event_count_mutex);
        return -1;
    } 
    else if (c >= 'a' && c <= 'z') 
    {
        event_count.button_event++;
        e->number.button_event = event_count.button_event;
        e->type = EVENT_BUTTON;
        e->data.key = c;
        e->processing_time = 2;  // BUTTON: 2 seconds
        printf("[Producer-%d] ✓ Created BUTTON   #%-3d '%c' (Process: 2s)\n", producer_id, e->number.button_event, c);
    }
    else 
    { 
        pthread_mutex_unlock(&event_count_mutex);
        return 0;
    }
    
    pthread_mutex_unlock(&event_count_mutex);
    return 1;
}

int event_come_delay=0;
static int current_event_index = 0;
static pthread_mutex_t event_index_mutex = PTHREAD_MUTEX_INITIALIZER;

void* producer_run(void* arg) 
{
    dataSendToProducer* data_rev_from_main=(dataSendToProducer*)arg;
    int ret;
    int thread_id = data_rev_from_main->thread_id;
    
    if(data_rev_from_main->command_from_user == 1) 
    {
        printf("[Producer %d] Mode 1 not implemented in simplified version\n", thread_id);
    }
    else if(data_rev_from_main->command_from_user == 2)
    {
        printf("[Producer %d] Mode 2 not implemented in simplified version\n", thread_id);
    }
    else if(data_rev_from_main->command_from_user == 3)
    {
        // Mode 3: Timed events from file - competitive mode
        int total_events_count = data_rev_from_main->num_timed_events;
        int start_time = time(NULL);
        
        printf("\n[Producer-%d] Started - competing for events...\n", thread_id);
        
        while(1)
        {
            // Get next event index atomically
            pthread_mutex_lock(&event_index_mutex);
            int my_event_idx = current_event_index;
            if(my_event_idx >= total_events_count) {
                pthread_mutex_unlock(&event_index_mutex);
                break;
            }
            current_event_index++;
            pthread_mutex_unlock(&event_index_mutex);
            
            TimedEvent* te = &data_rev_from_main->timed_events[my_event_idx];
            
            // Wait until the event timestamp
            int current_time = time(NULL) - start_time;
            if(te->timestamp > current_time) {
                sleep(te->timestamp - current_time);
            }
            
            Event* e = malloc(sizeof(Event));
            ret = create_events(te->event_char, e, thread_id);
            
            if(ret==-1)
            {
                queue_enqueue(data_rev_from_main->queue, e);
                break;
            }
            else if(ret==1) 
                queue_enqueue(data_rev_from_main->queue, e);
        }
        
        // Last producer creates shutdown events
        pthread_mutex_lock(&event_index_mutex);
        static int shutdown_sent = 0;
        if(!shutdown_sent) {
            shutdown_sent = 1;
            pthread_mutex_unlock(&event_index_mutex);
            
            printf("\n[Producer-%d] Sending %d SHUTDOWN signal(s)\n",
                thread_id, data_rev_from_main->total_consumers);
            for(int i = 0; i < data_rev_from_main->total_consumers; i++)
            {
                Event* e = malloc(sizeof(Event));
                pthread_mutex_lock(&event_count_mutex);
                event_count.shutdown_event++;
                e->number.shutdown_event = event_count.shutdown_event;
                e->type = EVENT_SHUTDOWN;
                e->processing_time = 0;
                pthread_mutex_unlock(&event_count_mutex);
                queue_enqueue(data_rev_from_main->queue, e);
            }
        } else {
            pthread_mutex_unlock(&event_index_mutex);
        }
    }
    
    printf("\n[Producer-%d] ✓ Finished\n", thread_id);
    return NULL;
}

