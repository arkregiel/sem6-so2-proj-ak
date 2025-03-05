#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* testThreadRoutine(void* arg)
{
    int threadID = *((int *)arg);
    printf("Hello from thread: %d\n", threadID);
    sleep(1);
    printf("Goodbye from thread: %d\n", threadID);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[5];
    
    for (int i = 0; i < 5; i++)
    {
        int res = pthread_create(&threads[i], NULL, &testThreadRoutine, &i);
        if (res != 0)
        {
            perror("pthread_create:");
            return -1;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }
}