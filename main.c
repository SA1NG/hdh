#include <pthread.h>
#include "queue.h"
#include "producer.h"
#include "consumer.h"
#include "event.h"
#include <stdlib.h> // srand()
#include <time.h>   // time() 

int num_producers = 1;
int num_consumers = 1;

int main() 
{
    srand(time(NULL)); 
    
    BlockingQueue queue;
    queue_init(&queue);
    array_key_init();
    
    char* line = NULL;  // For mode 2
    TimedEvent* timed_events_global = NULL; // For mode 3
    
    // Get number of producers and consumers
    printf("\n=== SYSTEM CONFIGURATION ===\n");
    printf("\nEnter number of PRODUCER threads (1-%d): ", MAX_PRODUCERS);
    scanf("%d", &num_producers);
    if(num_producers < 1) num_producers = 1;
    if(num_producers > MAX_PRODUCERS) num_producers = MAX_PRODUCERS;
    getchar();
    
    printf("  Enter number of CONSUMER threads (1-%d): ", MAX_CONSUMERS);
    scanf("%d", &num_consumers);
    if(num_consumers < 1) num_consumers = 1;
    if(num_consumers > MAX_CONSUMERS) num_consumers = MAX_CONSUMERS;
    getchar();
    
    printf("\n✓ Starting: %d Producer(s) × %d Consumer(s)\n",
        num_producers, num_consumers);
    
    int mode=0;
    printf("\n=== MODE SELECTION ===\n");
    printf("[1] Manual input (self-enter keys)\n");
    printf("[2] Auto mode (enter string directly)\n");
    printf("[3] File input (read from file)\n");
    printf("\nEnter mode (1/2/3): ");
    scanf("%d", &mode); 
    getchar();  // get '\n'
    
    while(mode < 1 || mode > 3) // choose mode until valid
    {
        printf("That mode is not supported, choose again!\n");
        printf("Enter mode (1/2/3): ");
        scanf("%d", &mode); 
        getchar();  // get '\n'
    }
    
    if(mode == 2)
    {
        size_t len = 0;
        ssize_t nread = getline(&line, &len, stdin);  // enter input string (accept space)
        if (nread > 0 && line[nread - 1] == '\n') line[nread - 1] = '\0'; 
        
        printf("Enter delay time for each events (s) -> ");   // enter delay time for each events
        scanf("%d", &event_come_delay); 
        getchar();  // get '\n'
    }
    else if(mode == 3)
    {
        // Mode 3: Read timed events from file
        // Format: <timestamp> <char> <processing_time>
        // Example: 0 s 2  (at 0s, create sensor, takes 2s to process)
        char filename[256];
        printf("Enter input filename: ");
        if(fgets(filename, sizeof(filename), stdin) != NULL) {
            // Remove newline
            size_t len = strlen(filename);
            if(len > 0 && filename[len-1] == '\n') filename[len-1] = '\0';
            
            FILE* file = fopen(filename, "r");
            if(file == NULL) {
                printf("Error: Cannot open file '%s'\n", filename);
                return 1;
            }
            
            // Parse file line by line
            char buffer[256];
            int line_count = 0;
            
            // First pass: count lines
            while(fgets(buffer, sizeof(buffer), file)) {
                if(buffer[0] != '\n' && buffer[0] != '#') line_count++;
            }
            
            // Allocate timed events array
            TimedEvent* timed_events = (TimedEvent*)malloc(sizeof(TimedEvent) * line_count);
            if(!timed_events) {
                printf("Error: Memory allocation failed\n");
                fclose(file);
                return 1;
            }
            
            // Second pass: parse events
            fseek(file, 0, SEEK_SET);
            int idx = 0;
            while(fgets(buffer, sizeof(buffer), file) && idx < line_count) {
                if(buffer[0] == '\n' || buffer[0] == '#') continue;
                
                int timestamp;
                char event_char;
                if(sscanf(buffer, "%d %c", &timestamp, &event_char) == 2) {
                    timed_events[idx].timestamp = timestamp;
                    timed_events[idx].event_char = event_char;
                    idx++;
                }
            }
            fclose(file);
            
            printf("\n✓ File loaded: %d timed events\n\n", idx);
            
            // Store globally for later use
            timed_events_global = timed_events;
            event_come_delay = idx; // Reuse this variable to store count
        }
    }
    
    // Create multiple producer threads
    pthread_t producers[MAX_PRODUCERS];
    dataSendToProducer producer_data[MAX_PRODUCERS];
    
    for(int i = 0; i < num_producers; i++)
    {
        producer_data[i].queue = &queue;
        producer_data[i].command_from_user = mode;
        producer_data[i].string_from_user = line;  // For mode 2
        
        // For mode 3, set timed events
        if(mode == 3) {
            producer_data[i].timed_events = timed_events_global;
            producer_data[i].num_timed_events = event_come_delay; // Reused variable
        } else {
            producer_data[i].timed_events = NULL;
            producer_data[i].num_timed_events = 0;
        }
        
        producer_data[i].thread_id = i + 1;
        producer_data[i].total_producers = num_producers;
        producer_data[i].total_consumers = num_consumers;
        pthread_create(&producers[i], NULL, producer_run, &producer_data[i]);
    }
    
    // Create multiple consumer threads
    pthread_t consumers[MAX_CONSUMERS];
    dataSendToConsumer consumer_data[MAX_CONSUMERS];
    
    for(int i = 0; i < num_consumers; i++)
    {
        consumer_data[i].queue = &queue;
        consumer_data[i].thread_id = i + 1;
        pthread_create(&consumers[i], NULL, consumer_run, &consumer_data[i]);
    }
    
    // Wait for all producer threads to finish
    for(int i = 0; i < num_producers; i++)
    {
        pthread_join(producers[i], NULL);
    }
    
    queue_stop(&queue);  // Stop queue after all producers done
    
    // Wait for all consumer threads to finish
    for(int i = 0; i < num_consumers; i++)
    {
        pthread_join(consumers[i], NULL);
    }
    
    queue_destroy(&queue);
    if(line) free(line);
    
    return 0;
}

