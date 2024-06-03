CC = mpicc
CFLAGS = -Wall
TARGET = mpi_master_slave
FILE = mpi_master_slave

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(FILE).c
	$(CC) $(CFLAGS) $(FILE).c -o $(TARGET) arduino_lib.c

run:
	mpirun -np 3 ./$(TARGET)

runf:
	mpirun -np 3 -hostfile my_host ./$(TARGET)

clean:
	rm -f $(TARGET)

comp:
	gcc -o main main.c

main:
	./main