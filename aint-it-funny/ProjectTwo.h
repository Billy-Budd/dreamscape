/* ProjectTwo.h
 * John Lawler | JML190001 | CS4348.501
 *
 * Header file for project 2. Contains important constants, declarations, and function
 * prototypes. Refer to sub-files for more information on functionality and use. 
*/

#ifndef PROJECTTWO_H_
#define PROJECTTWO_H_

// include for io and other basic c++ functionality
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime> 

// required for creation of semaphores and use of pthreads to interact + keep track of semaphores
#include <pthread.h>
#include <semaphore.h>

#define TOTAL_CUSTOMERS 25 // change this number to alter the number of customers the hotel is taking
#define FRONTDESK_EMPLOYEES 2 // req doc said 2 front desk employees, alter if you would like to change
#define BELLHOP_EMPLOYEES 2 // req doc said 2 bellhops, alter if you would like to change

// struct to store all relevant customer info
// values are -1 to show that nothing has been assigned
struct customer {
    int room = -1; // room assigned
    int bags = -1; // number of bags
    int frontdesk = -1; // frontdesk assignment
    int bellhop = -1; // bellhop assignment
};

// create semaphores
extern sem_t frontdesk_ready,                // frontdesk is ready for customer           
            bellhop_ready,                   // bellhop is ready for customer
            checked_in,                      // customer is done with frontdesk
            customer_exchanged,              // repeated semaphore to let bellhop or frontdesk know customer is done
            get_bellhop,                     // attach a bellhop to a customeer
            get_bags[TOTAL_CUSTOMERS],       // each customer needs a semaphore tied to their bags
            give_bags[TOTAL_CUSTOMERS],      // each customer needs a semaphore tied to giving bags
            enters_room[TOTAL_CUSTOMERS],    // each customer enters a separate room
            give_tip[TOTAL_CUSTOMERS],       // each customer gives separate tips
            give_room[TOTAL_CUSTOMERS];      // each customer receives a different room from frontdesk

// integer to keep track of customer id and room id
extern int shared_customer, current_room;

// an array of all the new customers in the hotel
extern customer customers[TOTAL_CUSTOMERS];

// prototypes for the printing functions to show semaphore/thread interactions

void GetBellhop(int);           // attach bellhop
void GetBags(int, int);         // get bags from customer
void GiveBags(int, int);        // give bags back to customer
void ReceiveBags(int, int);     // customer receives bags from bellhop

void GetRoom(int, int, int);    // customer gets room from front desk
void GiveRoom(int,int, int);    // frontdesk gives room to customer

void CheckIn(int, int);         // initialize check in 
void EnterRoom(int, int);       // customer enters room
void Retire(int);               // customer retires for the night

// semaphore functionality
void Wait(sem_t&);
void Post(sem_t&);
void InitializeSemaphore(sem_t&, int);

// create a random amount of bags [0,5] for a customer
int BagGenerator();

// create seed for BagGenerator
void SeedCreation();

// entity creation with threads
void *Customer(void*);
void *Frontdesk(void*);
void *Bellhop(void*);

// thread joining
void JoinThreads(pthread_t[], int);

#endif // PROCJECTTWO_H_