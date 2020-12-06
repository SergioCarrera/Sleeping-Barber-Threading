#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

// ==== sleepingStylistSem.c ==== //
# define CHAIRS 7
# define DELAY 1000000 // adjust this value
# define NUMBER_OF_CUSTOMERS 120

sem_t mutex, stylists, customers;
int waiting = 0;
int haircutsCompleted = 1;
int current = 1;

void stylist(void)
{
    int j;
    while(1){

        sem_wait(&customers);
        sem_wait(&mutex);
        // down(&customers);
        // down(&mutex);
        waiting = waiting - 1;
        sem_post(&stylists);
        sem_post(&mutex);
        // up(&stylist);
        // up(&mutex);
        printf("Customer Number: %d\n", haircutsCompleted);
        printf("Cutting hair\n");
        for(j = 0; j < DELAY; j++); // cut hair
        haircutsCompleted += 1;

        if(haircutsCompleted == NUMBER_OF_CUSTOMERS && waiting == 0) {
          break;
        }
    }
}

void customer(void)
{
    int j;
    while(1){
        //down(&mutex);
        sem_wait(&mutex);
        if(waiting < CHAIRS){
            waiting = waiting + 1;
            printf("Number of customers waiting is: %d\n", waiting);
            printf("Number of chairs available: %d\n", (CHAIRS - waiting));
            current++;
            sem_post(&customers);
            sem_post(&mutex);
            sem_wait(&stylists);
            break;
        }
        else{
            //up(&mutex);
            sem_post(&mutex);
            printf("Salon is full. This Customer tried to enter, but will come back. Customer Number: %d\n", current);
            for(j = 0; j < DELAY; j++); // go shopping
        }
    }
}

void main(void)
{
    sem_init(&mutex, 0, 1);
    sem_init(&stylists, 0, 0);
    sem_init(&customers, 0, 0);

    pthread_t customerThread[NUMBER_OF_CUSTOMERS];
    pthread_t stylistThread;

    int i;
    int j;
    // and a stylist thread. Don't forget to join threads
    pthread_create(&stylistThread, NULL, (void *) stylist, NULL);

    // create a specified number of customer threads
    for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
      pthread_create(&customerThread[i], NULL, (void *) customer, NULL);
    }

    for(j = 0; j < NUMBER_OF_CUSTOMERS; j++) {
      pthread_join(customerThread[j], NULL);
    }

    printf("Customers serviced: %d\n", haircutsCompleted);

    sem_destroy(&mutex);
    sem_destroy(&stylists);
    sem_destroy(&customers);
}
