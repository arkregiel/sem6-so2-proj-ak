#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "philosophers.h"


/* global variables */

// mutex for securing critical section
pthread_mutex_t mutexChopsticks;

// variable for waking up philosophers
pthread_cond_t *condPhilosophers;

// philosophers threads
pthread_t *philosopherThreads;

// number of philosophers (threads)
int philosophersCount;

// states of philosophers
PhilosopherState *philosophers;

// condition for philosophers to keep running.
// Set to 0 after SIGINT to break out of the loop
int alive = 1;

/* main function */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <philosophers-count>", argv[0]);
        return EXIT_FAILURE;
    }

    philosophersCount = atoi(argv[1]);

    if (philosophersCount <= 0) {
        printf("[!!] Invalid philosophers count: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    signal(SIGINT, &shutdown);

    // memory allocation and initialization of variables
    philosopherThreads = (pthread_t *)malloc(philosophersCount * sizeof(pthread_t));
    philosophers = (PhilosopherState *)malloc(philosophersCount * sizeof(PhilosopherState));
    condPhilosophers = (pthread_cond_t *)malloc(philosophersCount * sizeof(pthread_cond_t));

    pthread_mutex_init(&mutexChopsticks, NULL);
    
    // IDs of philosopher threads
    int* philosophersIDs = (int *)malloc(philosophersCount * sizeof(int));
    for (int i = 0; i < philosophersCount; i++) {
        philosophersIDs[i] = i;
    }

    srand(time(NULL));

    // starting threads
    for (int i = 0; i < philosophersCount; i++) {
        printf("[*] Creating philosopher %d...\n", i);
        fflush(stdout);
        pthread_cond_init(&condPhilosophers[i], NULL);
        if (pthread_create(&philosopherThreads[i], NULL, &philosopherRoutine, &philosophersIDs[i]) != 0) {
            perror("pthread_create:");
            return EXIT_FAILURE;
        }
    }

    // waiting for threads to finish
    for (int i = 0; i < philosophersCount; i++) {
        pthread_join(philosopherThreads[i], NULL);
    }

    // cleanup

    printf("[*] Cleaning...\n");

    pthread_mutex_destroy(&mutexChopsticks);

    for (int i = 0; i < philosophersCount; i++) {
        pthread_cond_destroy(&condPhilosophers[i]);
    }

    // releasing heap memory
    delete(philosophersIDs);
    delete(philosopherThreads);
    delete(philosophers);
    delete(condPhilosophers);

    printf("Goodbye");

    return EXIT_SUCCESS;
}

void delete(void *object) {
    if (object != NULL) {
        free(object);
    }
}

void shutdown(int signum) {
    printf("[*] Singal received, waiting for threads to finish...\n");
    fflush(stdout);
    alive = 0;
}

void* philosopherRoutine(void *arg) {
    int philosopherID = *((int *)arg);

    printf("[ %d ] Start...\n", philosopherID);
    fflush(stdout);

    while (alive) {
        printf("[ %d ] Thinking...\n", philosopherID);
        fflush(stdout);
        sleep(rand() % 2 + 1);
        #ifdef VERBOSE
        printf("[ %d ] Trying to pickup chopsticks...\n", philosopherID);
        fflush(stdout);
        #endif
        eat(philosopherID);
    }

    return NULL;
}

void eat(int philosopherID) {

    takeChopsticks(philosopherID);

    printf("[ %d ] Eating...\n", philosopherID);
    fflush(stdout);

    sleep(rand() % 3 + 1);

    #ifdef VERBOSE
    printf("[ %d ] Finished eating, releasing chopsticks...\n", philosopherID);
    fflush(stdout);
    #endif

    releaseChopsticks(philosopherID);

    #ifdef VERBOSE
    printf("[ %d ] Chopsticks released\n", philosopherID);
    fflush(stdout);
    #endif
}

void checkPhilosopher(int philosopherID) {
    #ifdef VERBOSE
    printf("[+] Testing state of philosopher %d\n", philosopherID);
    fflush(stdout);
    #endif

    if (philosophers[philosopherID] == STARVING &&
        philosophers[(philosopherID + philosophersCount - 1) % philosophersCount] != EATING &&  // philosopher on the left
        philosophers[(philosopherID + 1) % philosophersCount] != EATING) {                      // philosopher on the right
        
        philosophers[philosopherID] = EATING;   // pickup chopsticks
        pthread_cond_signal(&condPhilosophers[philosopherID]);  // wake up philosopher's thread

        #ifdef VERBOSE
        printf("[+] Philosopher %d can eat\n", philosopherID);
        fflush(stdout);
        #endif
    }
}

void takeChopsticks(int philosopherID) {

    pthread_mutex_lock(&mutexChopsticks);       // beginning of critical section

    philosophers[philosopherID] = STARVING;
    checkPhilosopher(philosopherID);

    // wait for opportunity to eat
    while (philosophers[philosopherID] != EATING) {
        printf("[ %d ] Waiting...\n", philosopherID);
        fflush(stdout);
        pthread_cond_wait(&condPhilosophers[philosopherID], &mutexChopsticks);
    }

    pthread_mutex_unlock(&mutexChopsticks);     // end of critical section
}

void releaseChopsticks(int philosopherID) {

    pthread_mutex_lock(&mutexChopsticks);       // beginning of critical section

    philosophers[philosopherID] = THINKING;
    checkPhilosopher((philosopherID + philosophersCount - 1) % philosophersCount);  // wake up philosopher on the left
    checkPhilosopher((philosopherID + 1) % philosophersCount);                      // wake up philosopher on the right

    pthread_mutex_unlock(&mutexChopsticks);     // end of critical section
}
