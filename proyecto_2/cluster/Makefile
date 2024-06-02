CC = mpicc
CFLAGS = -Wall
TARGET = mpi_master_slave
FILE = mpi_master_slave

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(FILE).c
	$(CC) $(CFLAGS) $(FILE).c -o $(TARGET)

run:
	mpirun -np 3 ./$(TARGET)

clean:
	rm -f $(TARGET)
