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
├── Makefile # Compilation and execution instructions
├── main.c # Main orchestration logic
├── main.h
├── request_handler.c # Thread function logic for handling requests
├── request_handler.h
├── utils.c # Logging, time utilities, etc.
├── utils.h
├── input.txt # Input file with request group data
└── log.txt # Auto-generated log file with simulation output



---

## 🛠️ Build & Run

Ensure you have `gcc` and POSIX thread support installed. Then:

```bash
# Compile the project
make

# Run the simulation
make run

