#ifndef MAIN_H
#define MAIN_H

#define _POSIX_C_SOURCE 200809L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>

#define MAX_LINE 256
#define MAX_GROUPS 100
#define MAX_REQUESTS_PER_GROUP 100
#define PAYMENT_PROCESSING_TIME 3 // seconds

// Structure declarations
typedef struct {
    int customer_id;
    int product_id;
    int quantity;
} ProductRequest;

typedef struct {
    ProductRequest* requests[MAX_REQUESTS_PER_GROUP];
    int count;
} RequestGroup;

typedef struct {
    ProductRequest* request;
    long reservation_start_time_stamp; // in milliseconds
    long expiration_time_stamp; // in milliseconds
    int retry_request; // -1 if none
} ThreadArgs;

typedef struct {
    int customer_id;
    long expires_at;
    bool is_purchased;
    int retry_customer_id;     // -1 if none
    bool retry_scheduled;      // prevents duplicate retries
} Reservation;

// Global variables declaration (defined in main.c)
extern int num_customers, num_products, reservation_timeout_ms, max_concurrent_payments;
extern pthread_mutex_t stock_mutex;
extern int *stock;
extern int *initial_stock;
extern RequestGroup request_groups[MAX_GROUPS];
extern int group_count;
extern pthread_mutex_t payment_mutex;
extern pthread_cond_t payment_cond;
extern int current_payments;
extern pthread_mutex_t reservations_mutex;
extern Reservation* reservations;
extern pthread_mutex_t log_mutex;

#endif /* MAIN_H */