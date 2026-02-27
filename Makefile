# Compiler settings
CC = gcc
# -D_FILE_OFFSET_BITS=64 ensures support for files > 4GB on 32-bit systems
CFLAGS = -Wall -Wextra -O2 -D_FILE_OFFSET_BITS=64

# Targets
all: server sender

# Link the server executable
server: server.o
	$(CC) $(CFLAGS) -o server server.o

# Link the sender executable
sender: sender.o
	$(CC) $(CFLAGS) -o sender sender.o

# Compile source files into object files
%.o: %.c transfer.h
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled binaries and object files
clean:
	rm -f server sender *.o received_output.dat

.PHONY: all clean
