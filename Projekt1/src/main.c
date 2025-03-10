#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

// if you want more verbosity, uncomment following line:
// #define VERBOSE

// representation of philosopher's current state
typedef enum {
    EATING,
    THINKING,
    STARVING
} PhilosopherState;

/* Funcion prototypes */

// deleting dynamically allocated object from memory
void delete(void *object);

// routine for a single philosopher's thread
void* philosopherRoutine(void *arg);

// function reperesenting eating
void eat(int philosopherID);

/*
checking philosopher's state. If philosopher is hungry and philosophers
on his left and right aren't eating - philosopher starts eating
*/
void checkPhilosopher(int philosopherID);

// pickup chopsticks
void takeChopsticks(int philosopherID);

// realease chopsticks
void releaseChopsticks(int philosopherID);


/* global variables */

// mutex for securing critical section
pthread_mutex_t mutexChopsticks;

// variable for waking up philosophers
pthread_cond_t *condPhilosophers;

// number of philosophers (threads)
int philosophersCount;

// states of philosophers
PhilosopherState *philosophers;

/* main function */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <philosophers-count>", argv[0]);
        return EXIT_FAILURE;
    }

    philosophersCount = atoi(argv[1]);

    if (philosophersCount <= 0) {
        printf("[!!] Invalid philosophers count\n");
        return EXIT_FAILURE;
    }

    // memory allocation and initialization of variables
    pthread_t *philosopherThreads = (pthread_t *)malloc(philosophersCount * sizeof(pthread_t));
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

    // cleaning

    // waiting for threads to finish
    for (int i = 0; i < philosophersCount; i++) {
        pthread_join(philosopherThreads[i], NULL);
    }

    pthread_mutex_destroy(&mutexChopsticks);

    for (int i = 0; i < philosophersCount; i++) {
        pthread_cond_destroy(&condPhilosophers[i]);
    }

    // releasing heap memory
    delete(philosophersIDs);
    delete(philosopherThreads);
    delete(philosophers);
    delete(condPhilosophers);

    return EXIT_SUCCESS;
}

void delete(void *object) {
    if (object != NULL) {
        free(object);
    }
}

void* philosopherRoutine(void *arg) {
    int philosopherID = *((int *)arg);

    printf("[ %d ] Start...\n", philosopherID);
    fflush(stdout);

    while (1) {
        printf("[+ %d +] Thinking...\n", philosopherID);
        fflush(stdout);
        sleep(rand() % 2 + 1);
        #ifdef VERBOSE
        printf("[ %d ] Trying to pickup chopsticks...\n", philosopherID);
        fflush(stdout);
        #endif
        eat(philosopherID);
    }

}

void eat(int philosopherID) {

    takeChopsticks(philosopherID);

    printf("[+ %d +] Eating...\n", philosopherID);
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
        printf("[+ %d +] Waiting...\n", philosopherID);
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
