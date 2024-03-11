#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX_USERS 10
#define TOTAL_USERS 50

int balance = 0;
sem_t* semaphore;


// Thread function to simulate connection to database
void *connect(void *arg)
{
    int threadNumber = *((int *) arg);
    int x, tmp;

    // *** Start of Critical Region ***
    // lock it down for 10 threads at a time
    sem_wait(semaphore);

    srand(time(NULL)); // Seed the random number generator with current time
    int random_time = rand() % 10 + 1; // Generate a random number between 1 and 10
    printf("Thread #%d entered connection pool and waiting %d...", threadNumber, random_time);
    tmp = balance;
    tmp = tmp + 1;
    balance = tmp;

    // *** End of Critical region ***
    // Unlock
    sem_post(semaphore);
    
    printf("Thread #%d exiting connection pool\n", threadNumber);
    return NULL;
}

// Main: create threads (users) who want to connect to database
int main()
{
    pthread_t tid[TOTAL_USERS];

    // Create a semaphore to be used in the critical region, only 10 allowed at a time
    semaphore = sem_open("Semaphore", O_CREAT, 00644, 10);
    if(semaphore == SEM_FAILED)
    {
        printf("\n ERROR creating semaphore\n");
        exit(1);
    }

    // Create threads (users) to connect to a database
    for(int i = 0; i < TOTAL_USERS; ++i)
    {
        if(pthread_create(&tid[i], NULL, connect, NULL))
        {
            // if error
            printf("\n ERROR creating deposit thread %d\n", i);
            exit(1);
        }
        else printf("Thread %d created\n", tid[i]);
    }

    // Wait for threads (users) to finish 
    for(int i = 0; i < TOTAL_USERS; ++i)
    {    
        if(pthread_join(tid[i], NULL))
        {
            printf("\n ERROR joining thread %d\n", tid[i]);
            exit(1);
        }  
        else printf("Thread %d finished\n", tid[i]);  
    }


    // Cleanup threads and semaphore
    sem_close(semaphore);
    pthread_exit(NULL);
    
}