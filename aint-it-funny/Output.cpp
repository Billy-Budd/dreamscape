/* Output.cpp
 * John Lawler | JML190001 | CS4348.501
 * 
 * File with functions to print out statements according to requirements document.
*/
#include "ProjectTwo.h"

// customer_id requests a bellhop
void GetBellhop(int customer_id) {
    printf("Guest %d requests help with bags\n", customer_id);
}

// bellhop retrieves bags from cutstomer_id
void GetBags(int bellhop, int customer_id) {
    printf("Bellhop %d receives bags from guest %d\n", bellhop, customer_id);
}

// bellhop returns bags to customer_id
void GiveBags(int bellhop, int customer_id) {
    printf("Bellhop %d delivers bags to guest %d\n", bellhop, customer_id);
}

// customer_id receives bags from bellhop
void ReceiveBags(int customer_id, int bellhop) {
    printf("Guest %d receives bags from bellhop %d and gives tip\n", customer_id, bellhop);
}

// customer_id receives key for room from a frontdesk
void GetRoom(int customer_id, int room, int frontdesk) {
    printf("Guest %d receives room key for room %d from front desk employee %d\n", customer_id, room, frontdesk);
}

// frontdesk employee registers guest in a room
void GiveRoom(int frontdesk, int customer_id, int room) {
    printf("Front desk employee %d registers guest %d and assigns room %d\n", frontdesk, customer_id, room);
}

// customer_id is checked in with # of bags
void CheckIn(int customer_id, int bags) {
    printf("Guest %d enters hotel with %d bag(s)\n", customer_id, bags);
}

// customer_id enters a room
void EnterRoom(int customer_id, int room) {
    printf("Guest %d enters room %d\n", customer_id, room);
}

// customer_id retires
void Retire(int customer_id) {
    printf("Guest %d retires for the evening\n", customer_id);
}