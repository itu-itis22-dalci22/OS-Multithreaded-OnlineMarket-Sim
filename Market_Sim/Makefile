CC = gcc
CFLAGS = -Wall -pthread
DEPS = main.h utils.h reservation.h payment.h request_handler.h
OBJ = main.o utils.o reservation.o payment.o request_handler.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shopping_simulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean run

clean:
	rm -f *.o shopping_simulator log.txt

run: shopping_simulator
	./shopping_simulator