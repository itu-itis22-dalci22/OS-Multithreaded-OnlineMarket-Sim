#include "utils.h"

long get_current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

void log_message(const char* format, ...) {
    pthread_mutex_lock(&log_mutex);
    
    // Get current timestamp
    long current_time = get_current_time_ms();
    
    // Open log file in append mode
    FILE* log_file = fopen("log.txt", "a");
    if (log_file) {
        // Format timestamp as [timestamp]
        fprintf(log_file, "[%ld] ", current_time);
        
        // Write formatted message to log file
        va_list args;
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        
        fprintf(log_file, "\n");
        fclose(log_file);
        
        // Also print to console for debugging with same format
        printf("[%ld] ", current_time);
        va_list console_args;
        va_start(console_args, format);
        vprintf(format, console_args);
        va_end(console_args);
        printf("\n");
    }
    
    pthread_mutex_unlock(&log_mutex);
}

char* display_stock() {
    char* stock_str = malloc(256);
    if (stock_str == NULL) {
        return NULL;
    }
    
    sprintf(stock_str, "Stock : [ product 0: %d ,", stock[0]);
    
    for (int i = 1; i < num_products; i++) {
        char product_str[32];
        sprintf(product_str, " product %d: %d", i, stock[i]);
        strcat(stock_str, product_str);
        
        if (i < num_products - 1) {
            strcat(stock_str, " ,");
        }
    }
    
    strcat(stock_str, "]");
    return stock_str;
}

void read_input_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Cannot open input file");
        exit(1);
    }

    char line[MAX_LINE];
    int in_config = 1;
    int current_group = -1;

    while (fgets(line, sizeof(line), fp)) {
        // Trim leading whitespace
        char *start = line;
        while (*start == ' ' || *start == '\t') start++;

        // Trim trailing newline, spaces, tabs, and \r
        char *end = start + strlen(start) - 1;
        while (end >= start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r'))
            *end-- = '\0';

        if (start[0] == '\0') {
            // Blank line
            current_group++;
            continue;
        }

        if (in_config) {
            if (strncmp(line, "num_customers=", 14) == 0)
                num_customers = atoi(line + 14);
            else if (strncmp(line, "num_products=", 13) == 0)
                num_products = atoi(line + 13);
            else if (strncmp(line, "reservation_timeout_ms=", 23) == 0)
                reservation_timeout_ms = atoi(line + 23);
            else if (strncmp(line, "max_concurrent_payments=", 24) == 0)
                max_concurrent_payments = atoi(line + 24);
            else if (strncmp(line, "initial_stock=", 14) == 0) {
                initial_stock = malloc(sizeof(int) * num_products);
                char *token = strtok(line + 14, ",");
                for (int i = 0; token && i < num_products; i++) {
                    initial_stock[i] = atoi(token);
                    token = strtok(NULL, ",");
                }
                in_config = 0; // done reading config
                current_group = 0;
            }
        } else {
            int cid, pid, qty;
            if (sscanf(line, "%d,%d,%d", &cid, &pid, &qty) == 3) {
                ProductRequest req = {cid, pid, qty};
                request_groups[current_group].requests[request_groups[current_group].count] = malloc(sizeof(ProductRequest));
                *request_groups[current_group].requests[request_groups[current_group].count] = req;
                request_groups[current_group].count++;
            }
        }
    }

    group_count = current_group + 1;
    fclose(fp);
}
