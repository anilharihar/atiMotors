# Compiler settings
CC = gcc
# -D_FILE_OFFSET_BITS=64 ensures support for files > 4GB on 32-bit systems
CFLAGS = -Wall -Wextra -O2 -D_FILE_OFFSET_BITS=64

# Targets
all: server receiver

# Link the server executable
server: server.o
	$(CC) $(CFLAGS) -o server server.o

# Link the receiver executable
receiver: receiver.o
	$(CC) $(CFLAGS) -o receiver receiver.o

# Compile source files into object files
%.o: %.c transfer.h
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled binaries and object files
clean:
	rm -f server receiver *.o received_output.dat

.PHONY: all clean
