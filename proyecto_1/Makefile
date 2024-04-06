CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: creator client reconstructor statistics

creator: creator.c
	$(CC) $(CFLAGS) -o creator creator.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

reconstructor: reconstructor.c
	$(CC) $(CFLAGS) -o reconstructor reconstructor.c

statistics: statistics.c
	$(CC) $(CFLAGS) -o statistics statistics.c

clean:
	rm -f creator client reconstructor statistics
