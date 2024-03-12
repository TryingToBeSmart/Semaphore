#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX_USERS 10
#define TOTAL_USERS 50

int currentAmountOfUsers = 0;
sem_t* semaphore;


// Thread function to simulate connection to database
void *connect(void *arg)
{
    int threadNumber = *((int *) arg); // get the int threadNumber from args
    int x, tmp;
    int random_time = rand() % 10 + 1; // Generate a random number between 1 and 10

    // *** Start of Critical Region ***
    // lock it down for 10 threads at a time
    sem_wait(semaphore);

    currentAmountOfUsers = currentAmountOfUsers + 1;
    printf("Current users: %d\n", currentAmountOfUsers);

    printf("Thread #%d entered connection pool and waiting for %d seconds...\n", 
        threadNumber, random_time);
    sleep(random_time); // Sleep for the random time

    printf("Thread #%d exiting connection pool\n", threadNumber);

    // decrement the current users within the critical section
    currentAmountOfUsers = currentAmountOfUsers - 1;

    printf("Current users: %d\n", currentAmountOfUsers);
 
    // *** End of Critical region ***
    // Unlock
    sem_post(semaphore);

    return NULL;
}

// Main: create threads (users) who want to connect to database
int main()
{
    pthread_t tid[TOTAL_USERS];
    int thread_args[TOTAL_USERS];
    
    srand(time(NULL)); // Seed the random number generator with current time

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
        thread_args[i] = i;
        if(pthread_create(&tid[i], NULL, connect, &thread_args[i]))
        {
            // if error
            printf("\n ERROR creating deposit thread %d\n", i);
            exit(1);
        }
        else printf("Thread %d created\n", i);
    }

    // Wait for threads (users) to finish 
    for(int i = 0; i < TOTAL_USERS; ++i)
    {    
        if(pthread_join(tid[i], NULL))
        {
            printf("\n ERROR joining thread %d\n", i);
            exit(1);
        }  
        else printf("Thread %d finished\n", i);  
    }


    // Cleanup threads and semaphore
    sem_close(semaphore);
    pthread_exit(NULL);
    
}
