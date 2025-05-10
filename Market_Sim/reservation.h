#ifndef RESERVATION_H
#define RESERVATION_H

#include "main.h"

// Functions for managing reservations
void save_the_last_reservation(ThreadArgs* data);
bool can_schedule_retry(int pid, int cid);

#endif /* RESERVATION_H */