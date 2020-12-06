#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

// ==== monitor.c ==== //
# define CHAIRS 7

typedef struct {
  int numberOfThreadsBlocked;
  sem_t suspendThread;
  sem_t sleep;
}cond;

cond stylistAvailable, customerAvailable;
int customer = 0;
int stylist = 0;
int salonIsFull = 0;
int salonIsEmpty = 0;
int totalHaircuts = 0;
sem_t mutex;
int chairs[CHAIRS];
int previouslyFull = 0;
int previouslyEmpty = 0;

void monitor()
{
  sem_init(&mutex, 0, 1);
  sem_init(&customerAvailable.sleep, 0, 0);
  sem_init(&stylistAvailable.sleep, 0, 0);
  sem_init(&stylistAvailable.suspendThread, 0, 1);
  sem_init(&customerAvailable.suspendThread, 0, 1);

  customerAvailable.numberOfThreadsBlocked = 0;
  stylistAvailable.numberOfThreadsBlocked = 0;
}

int count(cond *cv)
{
  return cv->numberOfThreadsBlocked;
}

void wait(cond *cv)
{
  sem_wait(&cv->suspendThread);
  cv->numberOfThreadsBlocked = cv->numberOfThreadsBlocked + 1;
  sem_post(&mutex);
  sem_post(&cv->suspendThread);
  sem_wait(&cv->sleep);
  sem_wait(&mutex);
}

void signal(cond *cv)
{
  sem_wait(&cv->suspendThread);
  if(count(cv) > 0) {
    cv->numberOfThreadsBlocked = cv->numberOfThreadsBlocked - 1;
    sem_post(&cv->sleep);
  }
  sem_post(&cv->suspendThread);
}

void mon_checkCustomer()
{
    sem_wait(&mutex);
    stylist = stylist + 1;
    signal(&stylistAvailable); // stylist's ready to cut hair
    if(customer == 0){ // do not use while here
        salonIsEmpty = salonIsEmpty + 1;
        wait(&customerAvailable);
    }
    customer = customer - 1;
    totalHaircuts = totalHaircuts + 1;
    sem_post(&mutex);
}

int mon_checkStylist()
// This function may have faults.
// If you think it does, you'll need to fix it.
{
    sem_wait(&mutex);
    int status = 0;
    if(customer < CHAIRS){
        customer = customer + 1;
        signal(&customerAvailable);
        if(stylist == 0){ // do not use while here
            wait(&stylistAvailable);
        }
        stylist = stylist - 1;
        status = 1;
    }
    sem_post(&mutex);
    return status;
}

void mon_debugPrint(void) // print how many customers are waiting
{
    sem_wait(&mutex);
    if(customer == 0 && previouslyEmpty == 0) {
      salonIsEmpty = salonIsEmpty + 1;
    }

    if(customer == 7 && previouslyFull == 0){
      salonIsFull = salonIsFull + 1;
    }

    for(int i = 0; i < CHAIRS; i++) {
      chairs[i] = 0;
    }

    for(int i = 0; i < customer; i++) {
      chairs[i] = 1;
    }

    if(customer == 7){
      previouslyFull = 1;
    } else {
      previouslyFull = 0;
    }

    if (customer == 0)
    {
      previouslyEmpty = 1;
    } else
    {
      previouslyEmpty = 0;
    }

    printf("[%d][%d][%d][%d][%d][%d][%d] => %d\n", chairs[0], chairs[1], chairs[2], chairs[3], chairs[4], chairs[5], chairs[6], customer);
    printf("Given haircuts = %d\n", totalHaircuts);
    printf("Salon full = %d times\n", salonIsFull);
    printf("Salon empty = %d times\n", salonIsEmpty);
    sem_post(&mutex);
}
