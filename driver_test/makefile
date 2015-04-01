EXECUTABLE = driver_test

SOURCES = driver_test.c scheduler_control.c

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb3 -std=c99
LDFLAGS = -lpthread

OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) *.o
