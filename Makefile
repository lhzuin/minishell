# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror
DEBUGFLAGS = -ggdb#-g -DDEBUG # -DDEBUG defines DEBUG in your source code
TARGET = shell
SRC = main.c
SHELL_SRC = minishell/shell.c
JOBS_SRC = minishell/jobs.c
PROCESS_SRC = minishell/process.c
PARSER_SRC = minishell/parser.c
UTILS_SRC = minishell/utils.c
OBJ = main.o shell.o jobs.o process.o parser.o utils.o
PROJECT_NAME = minishell


# Builds and runs default target
all: $(TARGET) run

# Builds and runs debug target
debug: CFLAGS += $(DEBUGFLAGS) # This appends the debug flags to your existing CFLAGS
debug: all

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
shell.o: $(SHELL_SRC)
	@echo "Generating object file for minishell"
	@echo "Usage: $(CC) $(CFLAGS) -c $(SHELL_SRC)"
	$(CC) $(CFLAGS) -c $(SHELL_SRC)

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

# Generates object file for parser
parser.o: $(PARSER_SRC)
	@echo "Generating object file for parser"
	@echo "Usage: $(CC) $(CFLAGS) -c $(PARSER_SRC)"
	$(CC) $(CFLAGS) -c $(PARSER_SRC)

# Generates object file for utils
utils.o: $(UTILS_SRC)
	@echo "Generating object file for utils"
	@echo "Usage: $(CC) $(CFLAGS) -c $(UTILS_SRC)"
	$(CC) $(CFLAGS) -c $(UTILS_SRC)

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
