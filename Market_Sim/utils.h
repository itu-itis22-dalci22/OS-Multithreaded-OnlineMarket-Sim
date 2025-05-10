#ifndef UTILS_H
#define UTILS_H

#include "main.h"

// Time-related utilities
long get_current_time_ms();

// Logging utilities
void log_message(const char* format, ...);
char* display_stock();

// File IO utilities
void read_input_file(const char *filename);

#endif /* UTILS_H */