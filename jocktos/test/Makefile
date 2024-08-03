# The compiler to use
CC = gcc

# The compiler flags to use
CFLAGS = -Wall -g

# The include dir (relative to the test directory)
INC = -I../inc

# The source dir (relative to the test directory)
SRCDIR = ../src

# Object files dir (relative to the test directory)
OBJDIR = build

# Executable name
EXECS = $(OBJDIR)/allocator.test

# Default target
all: $(OBJDIR) $(EXECS)

# Create build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)


# Compile source files into object files
$(OBJDIR)/allocator.test: test_allocator.c $(SRCDIR)/allocator.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/allocator.test test_allocator.c $(SRCDIR)/allocator.c $(INC)


# Clean up build artifacts
clean:
	rm -rf $(OBJDIR)


# Run all .test files in OBJDIR
test: $(OBJDIR) $(EXECS)
	@for test in $(EXECS); do \
		$$test; \
	done

