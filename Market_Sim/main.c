#include "main.h"
#include "utils.h"
#include "request_handler.h"

// Initialize global variables
int num_customers, num_products, reservation_timeout_ms, max_concurrent_payments;
pthread_mutex_t stock_mutex = PTHREAD_MUTEX_INITIALIZER;
int *stock = NULL;
int *initial_stock = NULL;
RequestGroup request_groups[MAX_GROUPS];
int group_count = -1;
pthread_mutex_t payment_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t payment_cond = PTHREAD_COND_INITIALIZER;
int current_payments = 0;
pthread_mutex_t reservations_mutex = PTHREAD_MUTEX_INITIALIZER;
Reservation* reservations = NULL;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    // Initialize random seed
    srand(time(NULL));
    
    // Create or truncate log file before use
    FILE* log_file = fopen("log.txt", "w");
    if (log_file) {
        fclose(log_file);
    }
    
    read_input_file("input.txt");
    
    // Initialize reservations
    reservations = malloc(sizeof(Reservation) * num_products);
    for (int i = 0; i < num_products; i++) {
        reservations[i].customer_id = -1;
        reservations[i].expires_at = 0;
        reservations[i].is_purchased = false;
        reservations[i].retry_customer_id = -1;
        reservations[i].retry_scheduled = false;
    }
    
    // Initialize stock
    stock = malloc(sizeof(int) * num_products);
    for (int i = 0; i < num_products; i++) {
        stock[i] = initial_stock[i];
    }
    
    char initial_stock_str[256] = "Initial Stock : [";
    for (int i = 0; i < num_products; i++) {
        char product_str[32];
        if (i > 0) {
            sprintf(product_str, " product %d: %d", i, stock[i]);
            if (i < num_products - 1) {
                strcat(product_str, " ,");
            }
        } else {
            sprintf(product_str, " product %d: %d ,", i, stock[i]);
        }
        strcat(initial_stock_str, product_str);
    }
    strcat(initial_stock_str, "]");
    log_message("%s", initial_stock_str);
    
    // Process request groups
    for (int i = 0; i < group_count; i++) {
        int group_size = request_groups[i].count;
        pthread_t *group_threads = malloc(sizeof(pthread_t) * group_size);
        
        // Create threads for this group
        for (int j = 0; j < group_size; j++) {
            ThreadArgs* args = malloc(sizeof(ThreadArgs));
            args->request = request_groups[i].requests[j];
            args->reservation_start_time_stamp = 0;
            args->expiration_time_stamp = 0;
            args->retry_request = -1;
            
            if (pthread_create(&group_threads[j], NULL, handle_request, args) != 0) {
                perror("Failed to create thread");
                exit(1);
            }
            sleep(0.001); // Small delay between thread creation (1 millisecond)
        }
        
        // Wait for all threads in this group to finish
        for (int j = 0; j < group_size; j++) {
            pthread_join(group_threads[j], NULL);
        }
        
        free(group_threads);
    }
    
    // Cleanup
    free(stock);
    free(initial_stock);
    free(reservations);
    
    for (int i = 0; i < group_count; i++) {
        for (int j = 0; j < request_groups[i].count; j++) {
            free(request_groups[i].requests[j]);
        }
    }
    
    log_message("Simulation completed");
    return 0;
}