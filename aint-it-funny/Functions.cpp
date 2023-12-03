/* Functions.cpp
 * John Lawler | JML190001 | CS4348.501
 *
 * File contains major functions used in program.
 * The first are needed to randomly create a number of bags for a customer.
 * The second section are the thread functions, handling customers,
 * frontdesks, and bellhops.
 * The final third section catches semaphore errors for waiting, posting, and
 * initializing. 
*/
#include "ProjectTwo.h"

// function to set srand for BagGenerator
void SeedCreation(){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// function to create a random number of bags [0,5] for a customer
int BagGenerator() {
    //std::srand(std::time(nullptr)); // Seed the random number generator
    return std::rand() % 6; // Generate a random number between 0 and 5
}

// THREAD FUNCTIONS

// Handles every action a customer needs to complete in execution
void *Customer(void *arg) {

    // initialize customer pointer and free memory
    int *customer_ptr = (int*) arg;
    int customer = *customer_ptr;
    free(arg);

    // get a random number for customer bags
    customers[customer].bags = BagGenerator();

    // check in process
    CheckIn(customer, customers[customer].bags);

    // wait for semaphores
    Wait(frontdesk_ready);
    Wait(customer_exchanged);

    // update shared_customer
    shared_customer = customer;

    // post and wait
    Post(checked_in);
    Wait(give_room[customer]);

    // customer receives room
    GetRoom(customer, customers[customer].room, customers[customer].frontdesk);

    // bellhop takes bags if there are more than 2 bags
    if (customers[customer].bags > 2) {

        // get bellhop for customer
        GetBellhop(customer);

        // wait for semaphores
        Wait(bellhop_ready);
        Wait(customer_exchanged);

        // reupdate shared_customer because it was changed in another thread
        shared_customer = customer;

        // post and wait
        Post(get_bellhop);
        Wait(get_bags[customer]);
    }

    // customer enters room
    EnterRoom(customer, customers[customer].room);

    // bellhop gives bags back to a customer at their room
    if (customers[customer].bags > 2) {

        // post and wait
        Post(enters_room[customer]);
        Wait(give_bags[customer]);

        // customer receives bags and gives tip
        ReceiveBags(customer, customers[customer].bellhop);
        Post(give_tip[customer]);
    }

    // customer retires for evening
    Retire(customer);
    return NULL;
}

// handles everything done at the frontdesk, customer needs to check in and receive a room
void *Frontdesk(void *arg) {

    // create a pointer and initialize customer so that the thread can share information
    int *frontdesk_ptr = (int *) arg;
    int frontdesk = *frontdesk_ptr,
        customer;
    free(arg);

    // never stop this process until thee simulation ends
    while(true) {

        // post that the frontdesk is ready to handle a customer
        Post(frontdesk_ready);
        Wait(checked_in);

        // update customer with the shared memory shared_customer
        customer = shared_customer;

        // update customer information with their room and the frontdesk that helped them
        customers[customer].frontdesk = frontdesk;
        customers[customer].room = ++current_room;

        // customer is done with exchange
        Post(customer_exchanged);

        // customer recieves room key
        GiveRoom(customers[customer].frontdesk, customer, customers[customer].room);

        // post that the customer has been given a room
        Post(give_room[customer]);
    }
    return NULL; // never reaches but compiler has a warning so I added it 
}

// allows the bellhops to continuously bring customers their bags to their rooms
void *Bellhop(void *arg) {

    // create a pointer and initialize custoemr so that the thread can share information
    int *bellhop_ptr = (int *) arg;
    int bellhop = *bellhop_ptr,
        customer;
    free(arg);

    // never stop this process until the simulation ends
    while(true) {

        // post that the bellhop is ready and wait until somebody wants a bellhop
        Post(bellhop_ready);
        Wait(get_bellhop);

        // update customer with the shared memory shared_customer
        customer = shared_customer;

        // attach bellhop to customer
        customers[customer].bellhop = bellhop;

        // customer is done with exchange
        Post(customer_exchanged);

        // bellhop takes bags from customer
        GetBags(customers[customer].bellhop, customer);

        // post that bags have been taken, wait until customer enters their room
        Post(get_bags[customer]);
        Wait(enters_room[customer]);

        // return bags
        GiveBags(customers[customer].bellhop, customer);

        // post that bags have been returned, wait until customer has tipped
        Post(give_bags[customer]);
        Wait(give_tip[customer]);
    } 
    return NULL; // never reaches here but compiler has a warning so I added it
}

// join thread for customer once all of their tasks have been completed
void JoinThreads(pthread_t thread[], int thread_number) {

    // status for pthread_join function
    int status;

    // create enough threads to satisfy arguments
    for(int thread_count = 0; thread_count < thread_number; thread_count++) {

        // attempt to join pthreads
        status = pthread_join(thread[thread_count], NULL);

        // if status was not 0, then there was an error
        if (status != 0) {
            printf("Error, could not join threads\n");
            exit(4);
        }

        // joining complete, post and finish
        printf("Guest %d joined\n", thread_count);
    }
}

// SEMAPHORE FUNCTIONS

// attempt to have a semaphore wait, if it returns -1 there was an error and exit
void Wait(sem_t &semaphore) {
    if (sem_wait(&semaphore) == -1) {
        printf("Error, waiting for semaphore\n");
        exit(5);
    }
}

// attempt to post semaphore, if it returns -1 there was an error and exit
void Post(sem_t &semaphore) {
    if (sem_post(&semaphore) == -1) {
        printf("Error, sending semaphore\n");
        exit(6);
    }
}

// attempt to initialize semaphore, if it returns -1 there was an error and exit
void InitializeSemaphore(sem_t &semaphore, int initial_value) {
    if (sem_init(&semaphore, 0, initial_value) == -1) {
        printf("Error, failed to initialize semaphore\n");
        exit(7);
    }
}