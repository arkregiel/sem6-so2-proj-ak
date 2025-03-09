#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef enum {
    EATING,
    THINKING,
    STARVING
} PhilosopherState;

void delete(void *object);
void* philosopherRoutine(void *arg);
void eat(int philosopherID);
void checkPhilosopher(int philosopherID);
void takeChopsticks(int philosopherID);
void releaseChopsticks(int philosopherID);


/* global variables */
pthread_mutex_t mutexChopsticks;
pthread_cond_t *condPhilosophers;

int philosophersCount;
PhilosopherState *philosophers;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <philosophersCount", argv[0]);
        return EXIT_FAILURE;
    }

    philosophersCount = atoi(argv[1]);

    if (philosophersCount <= 0) {
        printf("[!!] Invalid philosophers count\n");
        return EXIT_FAILURE;
    }

    pthread_t *philosopherThreads = (pthread_t *)malloc(philosophersCount * sizeof(pthread_t));
    philosophers = (PhilosopherState *)malloc(philosophersCount * sizeof(PhilosopherState));
    condPhilosophers = (pthread_cond_t *)malloc(philosophersCount * sizeof(pthread_cond_t));

    pthread_mutex_init(&mutexChopsticks, NULL);
    
    int* philosophersIDs = (int *)malloc(philosophersCount * sizeof(int));
    for (int i = 0; i < philosophersCount; i++) {
        philosophersIDs[i] = i;
    }

    for (int i = 0; i < philosophersCount; i++) {
        printf("[*] Tworzenie filozofa %d...\n", i);
        fflush(stdout);
        pthread_cond_init(&condPhilosophers[i], NULL);
        if (pthread_create(&philosopherThreads[i], NULL, &philosopherRoutine, &philosophersIDs[i]) != 0) {
            perror("pthread_create:");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < philosophersCount; i++) {
        pthread_join(philosopherThreads[i], NULL);
    }

    pthread_mutex_destroy(&mutexChopsticks);

    for (int i = 0; i < philosophersCount; i++) {
        pthread_cond_destroy(&condPhilosophers[i]);
    }

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

        printf("[ %d ] Próbuję wziąć sztućce...\n", philosopherID);
        eat(philosopherID);
        fflush(stdout);
    }

}

void eat(int philosopherID) {

    takeChopsticks(philosopherID);
    printf("[ %d ] Jem...\n", philosopherID);
    fflush(stdout);
    sleep(3);

    printf("[ %d ] Skończyłem jeść, odkładam sztućce\n", philosopherID);
    fflush(stdout);

    releaseChopsticks(philosopherID);

    printf("[ %d ] Odłożyłem sztućce\n", philosopherID);
    fflush(stdout);
}

void checkPhilosopher(int philosopherID) {
    printf("[+] Sprawdzanie filozofa %d\n", philosopherID);
    fflush(stdout);
    if (philosophers[philosopherID] == STARVING &&
        philosophers[(philosopherID + philosophersCount - 1) % philosophersCount] != EATING &&
        philosophers[(philosopherID + 1) % philosophersCount] != EATING) {
        
        philosophers[philosopherID] = EATING;
        pthread_cond_signal(&condPhilosophers[philosopherID]);
        printf("[+] Filozof %d może jeść\n", philosopherID);
        fflush(stdout);
    }
}

void takeChopsticks(int philosopherID) {
    pthread_mutex_lock(&mutexChopsticks);

    philosophers[philosopherID] = STARVING;
    checkPhilosopher(philosopherID);

    while (philosophers[philosopherID] != EATING) {
        // printf("[ %d ] Czekam na sztućce...\n", philosopherID);
        fflush(stdout);
        pthread_cond_wait(&condPhilosophers[philosopherID], &mutexChopsticks);
    }

    pthread_mutex_unlock(&mutexChopsticks);
}

void releaseChopsticks(int philosopherID) {
    pthread_mutex_lock(&mutexChopsticks);

    philosophers[philosopherID] = THINKING;
    checkPhilosopher((philosopherID + philosophersCount - 1) % philosophersCount);
    checkPhilosopher((philosopherID + 1) % philosophersCount);
    printf("[ %d ] Zwolniłem sztućce\n", philosopherID);
    fflush(stdout);

    pthread_mutex_unlock(&mutexChopsticks);
}
