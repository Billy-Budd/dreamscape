/* ProjectTwo.cpp 1
 * John Lawler | JML190001 | CS4348.501
 *
 * Main file to handle the execution of the program. 
 * Program generates each step with relevant notes that detail the process
 * during execution. 
*/
#include "ProjectTwo.h"

// create semaphores (copied from header file)
sem_t frontdesk_ready,               // frontdesk is ready for customer           
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
int shared_customer, current_room;

// an array of all the new customers in the hotel
customer customers[TOTAL_CUSTOMERS];

int main() {

    // create a random seed to be used for bag generation
    SeedCreation();

    // set initial values outside of range
    shared_customer = -1;
    current_room = 0;

    // status checker
    int status;

    // pthread initialization to create threads for customers, frontdesks, and bellhops
    pthread_t customer_threads[TOTAL_CUSTOMERS];
    pthread_t frontdesk_threads[TOTAL_CUSTOMERS];
    pthread_t bellhop_threads[TOTAL_CUSTOMERS];

    // initialize semaphores needed (not including ones per customer)
    InitializeSemaphore(checked_in, 0);
    InitializeSemaphore(frontdesk_ready, 0);
    InitializeSemaphore(bellhop_ready, 0);
    InitializeSemaphore(customer_exchanged, 1);
    InitializeSemaphore(get_bellhop, 0);

    // initialize semaphores for events pertaining per customer (ie bags, entering a room)
    for(int i = 0; i < TOTAL_CUSTOMERS; ++i) {
        InitializeSemaphore(give_room[i], 0);
        InitializeSemaphore(get_bags[i], 0);
        InitializeSemaphore(give_bags[i], 0);
        InitializeSemaphore(enters_room[i], 0);
        InitializeSemaphore(give_tip[i], 0);
    }

    // required statement for start of simulation
    printf("Simulation starts\n");

    // create frontdesks according to number in header file
    for (int frontdesk_count = 0; frontdesk_count < FRONTDESK_EMPLOYEES; frontdesk_count++) {

        // create a pointer pertaining to each frontdesk employee
        int *frontdesk_employee_ptr = (int*)malloc(sizeof(int));
        *frontdesk_employee_ptr = frontdesk_count;

        // create a pthread and update status with returning value
        status = pthread_create(&frontdesk_threads[frontdesk_count], NULL, Frontdesk, (void*)frontdesk_employee_ptr);

        // print front desk employee creation
        printf("Front desk employee %d created\n", *frontdesk_employee_ptr);

        // if pthread did not create, exit
        if (status != 0) {
            printf("Front desk thread creation fail.\n");
            exit(1);
        }
    }

    // create bellhops according to number in header file
    for(int bellhop_count = 0; bellhop_count < BELLHOP_EMPLOYEES; bellhop_count++) {

        // create a pointer pertaining to each bellhop
        int *bellhop_employee_ptr = (int*)malloc(sizeof(int));
        *bellhop_employee_ptr = bellhop_count;

        // create a pthread and upadate status with returning value
        status = pthread_create(&bellhop_threads[bellhop_count], NULL, Bellhop, (void*)bellhop_employee_ptr);

        // print bellhop creation
        printf("Bellhop %d created\n", *bellhop_employee_ptr);

        // if pthread did not create or had error, exit
        if (status != 0) {
            printf("Bellhop thread creation fail.\n");
            exit(2);
        }
    }

    // create customers according to number in header file
    for (int customer_count = 0; customer_count < TOTAL_CUSTOMERS; customer_count++) {

        // create a poiinter pertaining to each customer
        int *customer_ptr = (int*)malloc(sizeof(int));
        *customer_ptr = customer_count;

        // create a pthread and update status with returning value
        status = pthread_create(&customer_threads[customer_count], NULL, Customer, (void*)customer_ptr);

        // print guest creation
        printf("Guest %d created\n", *customer_ptr);

        // if pthread did not create or had error, exit
        if (status != 0) {
            printf("Customer thread creation fail.\n");
            exit(3);
        }
    }

    // complete thread creation and join threads
    JoinThreads(customer_threads, TOTAL_CUSTOMERS);

    // required statement for end of simulation
    printf("Simulation ends\n\n");

    return 0;
}