#include "payment.h"
#include "utils.h"

void payment(ThreadArgs* data) {
    pthread_mutex_lock(&payment_mutex);
    
    long now = get_current_time_ms();
    long time_left = data->expiration_time_stamp - now;
    
    while (current_payments >= max_concurrent_payments) {
        log_message("Customer %d couldn't purchase product %d (qty: %d) and had to wait! (maximum number of concurrent payments reached!)",
           data->request->customer_id, data->request->product_id, data->request->quantity);
        
        if (time_left <= 0) {
            // Time out expired while waiting
            pthread_mutex_unlock(&payment_mutex);
            
            pthread_mutex_lock(&stock_mutex);
            stock[data->request->product_id] += data->request->quantity;
            char* stock_str = display_stock();
            log_message("Customer %d could not purchase product %d (qty: %d) in time. Timeout is expired!!! Product %d (qty: %d) returned to the stock! | %s", 
                data->request->customer_id, data->request->product_id, data->request->quantity,
                data->request->product_id, data->request->quantity, stock_str);
            free(stock_str);
            pthread_mutex_unlock(&stock_mutex);
            
            return;
        }
        
        // Prepare absolute time for timedwait
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += time_left / 1000;
        ts.tv_nsec += (time_left % 1000) * 1000000;
        
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        
        int wait_result = pthread_cond_timedwait(&payment_cond, &payment_mutex, &ts);
        
        // Check if timeout occurred during wait
        now = get_current_time_ms();
        time_left = data->expiration_time_stamp - now;
        
        if (time_left <= 0 || wait_result == ETIMEDOUT) {
            pthread_mutex_unlock(&payment_mutex);
            
            pthread_mutex_lock(&stock_mutex);
            stock[data->request->product_id] += data->request->quantity;
            char* stock_str = display_stock();
            log_message("Customer %d could not purchase product %d (qty: %d) in time. Timeout is expired!!! Product %d (qty: %d) returned to the stock! | %s", 
                data->request->customer_id, data->request->product_id, data->request->quantity,
                data->request->product_id, data->request->quantity, stock_str);
            free(stock_str);
            pthread_mutex_unlock(&stock_mutex);
            
            return;
        }
        else{
            char* stock_str = display_stock();
            log_message("Customer %d (automatically) retried purchasing product %d (qty: %d) | %s  // (checked for available cashier slot before timeout expired.) ",
                data->request->customer_id, data->request->product_id, 
                data->request->quantity, stock_str);
            free(stock_str);
        }
    }
    
    // Successfully acquired payment slot
    pthread_mutex_lock(&reservations_mutex);
    reservations[data->request->product_id].is_purchased = true;
    current_payments++;
    pthread_mutex_unlock(&reservations_mutex);
    pthread_mutex_unlock(&payment_mutex);
    
    sleep(PAYMENT_PROCESSING_TIME); // Simulate payment processing time
    
    pthread_mutex_lock(&payment_mutex);
    current_payments--;
    char* stock_str2 = display_stock();
    log_message("Customer %d purchased product %d (qty: %d) | %s", 
           data->request->customer_id, data->request->product_id, 
           data->request->quantity, stock_str2);
    free(stock_str2);
    pthread_cond_signal(&payment_cond);
    pthread_mutex_unlock(&payment_mutex);
}