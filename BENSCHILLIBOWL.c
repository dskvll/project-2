#include "BENSCHILLIBOWL.h"
 
#include <assert.h>
#include <stdlib.h>
#include <time.h>
 
bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);
 
MenuItem BENSCHILLIBOWLMenu[] = {
   "BensChilli",
   "BensHalfSmoke",
   "BensHotDog",
   "BensChilliCheeseFries",
   "BensShake",
   "BensHotCakes",
   "BensCake",
   "BensHamburger",
   "BensVeggieBurger",
   "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;
 
/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
   int randomItem = rand() % BENSCHILLIBOWLMenuLength;
   return BENSCHILLIBOWLMenu[randomItem];
}
 
/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */
 
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
   // Memory allocation
   BENSCHILLIBOWL *b = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
// Variable initialization
   b->current_size=0;
   b->orders=NULL;
   b->max_size = max_size;
   b->orders_handled=0;
   b->expected_num_orders = expected_num_orders;
  
   pthread_mutex_init(&(b->mutex), NULL);
   pthread_cond_init(&(b->can_add_orders), NULL);
   pthread_cond_init(&(b->can_get_orders), NULL);
    printf("Restaurant is open!\n");
   return b;
}
 
 
/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */
 
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
   printf("\nOrders handled = %d ", bcb->orders_handled);
   printf("\nOrders expected = %d", bcb->expected_num_orders);
   if(bcb->expected_num_orders == bcb->orders_handled){
      pthread_mutex_destroy(&bcb->mutex);
      pthread_cond_destroy(&bcb->can_add_orders);
      pthread_cond_destroy(&bcb->can_get_orders);
   }
   free(bcb);
   printf("\nRestaurant is closed!\n");
}
 
/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
   pthread_mutex_lock(&(bcb->mutex));
   while (IsFull(bcb) == true ){
      pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
   }
   bcb->current_size++;
   order->order_number = bcb->next_order_number++;
   AddOrderToBack(&(bcb->orders),order);
    pthread_cond_signal(&(bcb->can_get_orders));
   pthread_mutex_unlock(&(bcb->mutex));
   return order->order_number;
}
 
/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
   pthread_mutex_lock(&(bcb->mutex));
   while (IsEmpty(bcb) == true){
       pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
   }
   Order *order1 = bcb->orders;
   bcb->orders = bcb->orders->next;
   bcb->current_size--;
   bcb->orders_handled++;
   pthread_cond_signal(&(bcb->can_add_orders));
   if (bcb->orders_handled == bcb->expected_num_orders){
       pthread_cond_broadcast(&(bcb->can_add_orders));
   }
   pthread_mutex_unlock(&(bcb->mutex));
    return order1;
}
 
// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
 if (bcb->current_size == 0 )
   return true;
 return false ;
}
 
bool IsFull(BENSCHILLIBOWL* bcb) {
 if (bcb->current_size == bcb->max_size)
     return true;
 return false;
}
 
/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
 if (orders[0]==NULL){
   orders[0]=order;
   order->next = NULL;
 }
 Order *temp = orders[0];
 while (temp->next != NULL){
   temp = temp->next;
 }
 temp->next = order;
 order->next = NULL; // Order is not added to the back of list.
 return;
}
