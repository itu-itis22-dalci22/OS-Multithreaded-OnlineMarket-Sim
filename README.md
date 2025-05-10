<h1 align="center">🧵 Multithreaded Product Reservation & Payment System</h1>

<p align="center">
  <b>Operating Systems Project – Full Implementation</b><br>
  A synchronized, multithreaded simulation of a product reservation and payment platform using POSIX threads, mutexes, and condition variables.
</p>

---

## 🚀 Overview

This project simulates a multi-customer environment where each customer can reserve and purchase products concurrently. It ensures:

- 🧵 Thread-based request handling  
- 🔒 Mutex-guarded access to shared stock and reservations  
- ⏳ Reservation timeouts with retry logic  
- 💳 Limited concurrent payments using condition variables  
- 📜 Safe, timestamped logging of all events  

---

## 📁 Project Structure

.
├── Makefile # Compilation and execution instructions
├── main.c # Main orchestration logic
├── main.h
├── request_handler.c # Thread function logic for handling requests
├── request_handler.h
├── utils.c # Logging, time utilities, etc.
├── utils.h
├── input.txt # Input file with request group data
└── log.txt # Auto-generated log file with simulation output

yaml
Copy
Edit

---

## 🛠️ Build & Run

Ensure you have gcc and POSIX thread support installed. Then:

bash
# Compile the project
make

# Run the simulation
make run
The results will be logged in a freshly generated log.txt file.

📌 Features
✅ Fully modular C code (separated headers and source files)

🔐 Thread-safe stock and reservation updates

⏱ Dynamic timeout handling with retry support

🧾 Structured logging for reproducible testing

⚙️ Simple Makefile to build and run in one step

🧪 Sample Input Format
Example input.txt format:

sql
Copy
Edit
3 5 2000 2
GROUP
1 0
2 3
GROUP
0 2
1 1
3 4
Where:

3 = number of customers

5 = number of products

2000 = reservation timeout in milliseconds

2 = max concurrent payments

📓 Log Output
All key actions are logged in log.txt for debugging and verification.

Example:

csharp
Copy
Edit
[Time 12] Customer 1 reserved product 3
[Time 2050] Customer 1 payment succeeded
🧠 Concepts Used
pthread_create, pthread_join – thread management

pthread_mutex_t – mutual exclusion for shared resources

pthread_cond_t – for blocking/waking up threads

Dynamic memory management

Timestamping with gettimeofday()

📚 Report
Includes a detailed LaTeX-written report describing:

System architecture

Synchronization mechanisms

Timeout strategy and thread behavior

Design choices and trade-offs

Optional: Add report/ directory to include LaTeX .tex and compiled .pdf.

👤 Author
Your Name
Computer Engineering Student
📧 your.email@example.com
🌐 LinkedIn | GitHub
