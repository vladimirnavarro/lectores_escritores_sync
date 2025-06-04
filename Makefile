CC=gcc
CFLAGS=-Wall -pthread -O2

SRC=src
BIN=bin

all: $(BIN)/le_mutex_cond $(BIN)/le_busy_wait $(BIN)/le_semaphore

$(BIN)/le_mutex_cond: $(SRC)/le_mutex_cond.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN)/le_busy_wait: $(SRC)/le_busy_wait.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN)/le_semaphore: $(SRC)/le_semaphore.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BIN)/* *.o *.csv
