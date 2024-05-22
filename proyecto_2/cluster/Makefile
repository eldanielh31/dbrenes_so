CC = mpicc
CFLAGS = -Wall
TARGET = mpi_master_slave
FILE = TallerMPI

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(FILE).c
	$(CC) $(CFLAGS) $(FILE).c -o $(TARGET)

clean:
	rm -f $(TARGET)
