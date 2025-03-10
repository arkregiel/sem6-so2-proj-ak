#pragma once

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

// SIGINT signal handler
void shutdown(int signum);

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
