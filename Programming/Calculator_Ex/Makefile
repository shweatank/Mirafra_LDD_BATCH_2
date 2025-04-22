# Compiler
CC = gcc

# Source files
SRCS = main.c operations.c

# File name lists
PREPROCESSED = $(SRCS:.c=.i)
ASSEMBLIES   = $(SRCS:.c=.s)
OBJECTS      = $(SRCS:.c=.o)

# Final executable
TARGET = main_exe

# Default target: build everything
all: $(TARGET)

# Step 1: Preprocess (.c → .i)
%.i: %.c
	$(CC) -E $< -o $@

# Step 2: Compile (.i → .s)
%.s: %.i
	$(CC) -S $< -o $@

# Step 3: Assemble (.s → .o)
%.o: %.s
	$(CC) -c $< -o $@

# Step 4: Link all .o files to final executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Clean everything
clean:
	rm -f $(PREPROCESSED) $(ASSEMBLIES) $(OBJECTS) $(TARGET)
