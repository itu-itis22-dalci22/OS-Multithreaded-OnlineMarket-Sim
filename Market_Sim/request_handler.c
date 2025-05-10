#include "request_handler.h"
#include "utils.h"
#include "payment.h"
#include "reservation.h"

void* handle_request(void* arg) {
    ThreadArgs* data = (ThreadArgs*) arg;
    
    pthread_mutex_lock(&stock_mutex);
    bool has_stock = stock[data->request->product_id] >= data->request->quantity;
    pthread_mutex_unlock(&stock_mutex);
    
    if (has_stock) {
        // Reserve product
        pthread_mutex_lock(&stock_mutex);
        pthread_mutex_lock(&reservations_mutex);
        
        stock[data->request->product_id] -= data->request->quantity;
        data->reservation_start_time_stamp = get_current_time_ms();
        data->expiration_time_stamp = data->reservation_start_time_stamp + reservation_timeout_ms;
        save_the_last_reservation(data);
        
        char* stock_str = display_stock();
        log_message("Customer %d tried to add product %d (qty: %d) to cart | %s  // succeed",
           data->request->customer_id, data->request->product_id, 
           data->request->quantity, stock_str); 
        free(stock_str);
        
        pthread_mutex_unlock(&stock_mutex);
        pthread_mutex_unlock(&reservations_mutex);
        
        // Decide whether to purchase or let timeout expire
        int will_purchase = rand() % 2;
        if (will_purchase) {
            char* stock_str = display_stock();
            log_message("Customer %d attempted to purchase product %d (qty: %d) | %s", 
                data->request->customer_id, data->request->product_id,
                data->request->quantity, stock_str);
            free(stock_str);
            sleep(2); // Simulate processing time before payment
            payment(data);
        }
        else {
            long now = get_current_time_ms();
            long time_left = data->expiration_time_stamp - now;
            
            // Simulate waiting for timeout
            sleep(time_left / 1000); // Convert ms to seconds for sleep
            
            // Return product to stock
            pthread_mutex_lock(&stock_mutex);
            stock[data->request->product_id] += data->request->quantity;
            char* stock_str = display_stock();
            log_message("Customer %d timeout expired. Product %d (qty: %d) returned to the stock! | %s", 
                data->request->customer_id, data->request->product_id, data->request->quantity, stock_str);
            free(stock_str);
            pthread_mutex_unlock(&stock_mutex);
        }
    }
    else if (data->retry_request == -1) {
        // First attempt failed due to insufficient stock
        char* stock_str = display_stock();
        log_message("Customer %d tried to add product %d (qty: %d) but only %d units were available | %s  // failed",
           data->request->customer_id, data->request->product_id, 
           data->request->quantity, stock[data->request->product_id], stock_str);
        free(stock_str);
        
        data->reservation_start_time_stamp = get_current_time_ms();
        data->expiration_time_stamp = data->reservation_start_time_stamp + reservation_timeout_ms;
        
        pthread_mutex_lock(&reservations_mutex);
        bool can_retry = can_schedule_retry(data->request->product_id, data->request->customer_id);
        
        if (!can_retry) {
            char* stock_str = display_stock();
            log_message("Customer %d could NOT reserve product %d (stock: %d) — retry denied | %s  // failed",
                       data->request->customer_id, data->request->product_id, 
                       stock[data->request->product_id], stock_str);
            free(stock_str);
            pthread_mutex_unlock(&reservations_mutex);
        } 
        else {
            // Schedule retry
            long time_to_expire = reservations[data->request->product_id].expires_at - get_current_time_ms();
            
            reservations[data->request->product_id].retry_customer_id = data->request->customer_id;
            reservations[data->request->product_id].retry_scheduled = true;
            
            char* stock_str = display_stock();
            log_message("Customer %d tried to add product %d (qty: %d) but only %d units were available - retry scheduled in %ld ms | %s",
                data->request->customer_id, data->request->product_id, 
                data->request->quantity, stock[data->request->product_id], time_to_expire, stock_str);
            free(stock_str);
            
            pthread_mutex_unlock(&reservations_mutex);
            
            // Wait for the current reservation to expire
            long now = get_current_time_ms();
            long sleep_time_ms = reservations[data->request->product_id].expires_at - now;
            
            if (sleep_time_ms > 0) {
                log_message("Customer %d will retry in %ld ms", 
                           data->request->customer_id, sleep_time_ms);
                sleep(sleep_time_ms / 1000); // Sleep for milliseconds converted to seconds
            }
            
            // Create retry thread
            ThreadArgs* retry_args = malloc(sizeof(ThreadArgs));
            memcpy(retry_args, data, sizeof(ThreadArgs));
            retry_args->retry_request = 0;
            
            pthread_t retry_thread;
            pthread_create(&retry_thread, NULL, handle_request, retry_args);
            pthread_detach(retry_thread);
            
            pthread_mutex_lock(&reservations_mutex);
            reservations[data->request->product_id].retry_scheduled = false;
            reservations[data->request->product_id].retry_customer_id = -1;
            pthread_mutex_unlock(&reservations_mutex);
        }
    }
    else {
        // This is a retry attempt
        if (data->retry_request == 0) {
            char* stock_str = display_stock();
            log_message("Customer %d retry attempt failed - product already reserved (or purchased) by another customer | %s  // no more retry for this thread ",
                data->request->customer_id, stock_str);  
            free(stock_str);
        }
    }
    
    free(data);
    return NULL;
}