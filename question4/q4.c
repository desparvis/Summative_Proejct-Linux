#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BELT_CAPACITY 5
#define TOTAL_ITEMS 20

// Shared Buffer (Circular Buffer)
int conveyor_belt[BELT_CAPACITY];
int head = 0; // Points to the next slot to load (Producer)
int tail = 0; // Points to the next slot to dispatch (Consumer)
int current_belt_size = 0;

// Shared Counters
int total_loaded = 0;
int total_dispatched = 0;

// Synchronization Primitives
pthread_mutex_t belt_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t belt_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t belt_not_empty = PTHREAD_COND_INITIALIZER;

// --- PRODUCER THREAD (Conveyor Belt Loader) ---
void* loader_producer(void* arg) {
    for (int i = 1; i <= TOTAL_ITEMS; i++) {
        sleep(2); // Loading takes 2 seconds

        pthread_mutex_lock(&belt_mutex);

        // Wait if belt is full
        while (current_belt_size == BELT_CAPACITY) {
            printf("[LOADER] Belt Full. Waiting...\n");
            pthread_cond_wait(&belt_not_full, &belt_mutex);
        }

        // Add luggage to circular buffer
        conveyor_belt[head] = i;
        head = (head + 1) % BELT_CAPACITY;
        current_belt_size++;
        total_loaded++;

        printf("[LOADER] Loaded Luggage #%d. (Belt Size: %d)\n", i, current_belt_size);

        // Signal consumer that luggage is available
        pthread_cond_signal(&belt_not_empty);
        pthread_mutex_unlock(&belt_mutex);
    }
    return NULL;
}

// --- CONSUMER THREAD (Aircraft Loader) ---
void* aircraft_consumer(void* arg) {
    while (total_dispatched < TOTAL_ITEMS) {
        sleep(4); // Dispatching takes 4 seconds

        pthread_mutex_lock(&belt_mutex);

        // Wait if belt is empty
        while (current_belt_size == 0 && total_dispatched < TOTAL_ITEMS) {
            printf("[AIRCRAFT] Belt Empty. Waiting for luggage...\n");
            pthread_cond_wait(&belt_not_empty, &belt_mutex);
        }

        if (total_dispatched < TOTAL_ITEMS) {
            int item = conveyor_belt[tail];
            tail = (tail + 1) % BELT_CAPACITY;
            current_belt_size--;
            total_dispatched++;

            printf("[AIRCRAFT] Dispatched Luggage #%d. (Belt Size: %d)\n", item, current_belt_size);

            // Signal producer that space is available
            pthread_cond_signal(&belt_not_full);
        }

        pthread_mutex_unlock(&belt_mutex);
    }
    return NULL;
}

// --- MONITOR THREAD ---
void* monitoring_thread(void* arg) {
    while (total_dispatched < TOTAL_ITEMS) {
        sleep(5); // Run every 5 seconds

        pthread_mutex_lock(&belt_mutex);
        printf("\n========== MONITOR REPORT ==========\n");
        printf("Total Loaded:     %d\n", total_loaded);
        printf("Total Dispatched: %d\n", total_dispatched);
        printf("Current Belt Size: %d/5\n", current_belt_size);
        printf("====================================\n\n");
        pthread_mutex_unlock(&belt_mutex);
    }
    return NULL;
}

int main() {
    pthread_t producer, consumer, monitor;

    printf("--- Starting Airport Baggage System ---\n");

    pthread_create(&producer, NULL, loader_producer, NULL);
    pthread_create(&consumer, NULL, aircraft_consumer, NULL);
    pthread_create(&monitor, NULL, monitoring_thread, NULL);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    pthread_join(monitor, NULL);

    printf("--- Simulation Complete. All luggage loaded to aircraft. ---\n");

    return 0;
}