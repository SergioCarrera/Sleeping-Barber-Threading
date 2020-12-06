#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "monitor.h"

// ==== sleepingStylistMon.c ==== //
# define DELAY 10 // adjust this value
# define NUMBER_OF_CUSTOMERS 120


void stylistA(void)
{
    // add more variables as needed
    int j;
    while(1){
        mon_debugPrint();
        mon_checkCustomer();
        for(j = 0; j < DELAY; j++); // cut hair
    }
}

void customerA(void)
{
    // add more variables as needed
    int j;
    while(1){
        mon_debugPrint();
        if(mon_checkStylist()){
            break;
        }
        for(j = 0; j < DELAY; j++); // go shopping
    }
}

void main(void)
{
  monitor();

  pthread_t customerThread[NUMBER_OF_CUSTOMERS];
  pthread_t stylistThread;

  // and a stylist thread. Don't forget to join threads
  pthread_create(&stylistThread, NULL, (void *) stylistA, NULL);

  // create a specified number of customer threads
  for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_create(&customerThread[i], NULL, (void *) customerA, NULL);
  }

  for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    pthread_join(customerThread[i], NULL);
  }
  pthread_join(stylistThread, NULL);
  printf("Done.\n");
}
