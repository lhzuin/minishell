# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = shell
SRC = main.c
MINISHELL_SRC = minishell/minishell.c
JOBS_SRC = minishell/jobs.c
PROCESS_SRC = minishell/process.c
OBJ = main.o minishell.o jobs.o process.o
PROJECT_NAME = minishell


# Builds and runs default target
all: $(TARGET) run

# Builds default target
$(TARGET): $(OBJ)
	@echo "Building $(PROJECT_NAME) project"
	@echo "Usage: $(CC) $(CFLAGS) $(OBJ) -o $(TARGET)"
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Generates object file for main
main.o: $(SRC)
	@echo "Generating object file for main"
	@echo "Usage: $(CC) $(CFLAGS) -c $(SRC)"
	$(CC) $(CFLAGS) -c $(SRC)

# Generates object file for minishell
minishell.o: $(MINISHELL_SRC)
	@echo "Generating object file for minishell"
	@echo "Usage: $(CC) $(CFLAGS) -c $(MINISHELL_SRC)"
	$(CC) $(CFLAGS) -c $(MINISHELL_SRC)

# Generates object file for jobs
jobs.o: $(JOBS_SRC)
	@echo "Generating object file for jobs"
	@echo "Usage: $(CC) $(CFLAGS) -c $(JOBS_SRC)"
	$(CC) $(CFLAGS) -c $(JOBS_SRC)

# Generates object file for process
process.o: $(PROCESS_SRC)
	@echo "Generating object file for process"
	@echo "Usage: $(CC) $(CFLAGS) -c $(PROCESS_SRC)"
	$(CC) $(CFLAGS) -c $(PROCESS_SRC)

# Runs target
run: 
	@echo "Runs $(PROJECT_NAME) project previous build"
	@echo "Usage: ./$(TARGET)"
	./$(TARGET)

# Clean target
clean:
	@echo "Deleting executable for $(PROJECT_NAME) project"
	@echo "Usage: rm -f $(OBJ) $(TARGET)"
	rm -f $(OBJ) $(TARGET)

# Phony targets (not associated with files)
.PHONY: all run clean
