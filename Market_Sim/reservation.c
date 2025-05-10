#include "reservation.h"
#include "utils.h"

void save_the_last_reservation(ThreadArgs* data) {
    reservations[data->request->product_id].customer_id = data->request->customer_id;
    reservations[data->request->product_id].expires_at = data->expiration_time_stamp;
    reservations[data->request->product_id].is_purchased = false;
    reservations[data->request->product_id].retry_customer_id = -1;
    reservations[data->request->product_id].retry_scheduled = false;
}

bool can_schedule_retry(int pid, int cid) {
    Reservation* r = &reservations[pid];
    return !r->retry_scheduled &&
           !r->is_purchased &&
           r->customer_id != -1;
}