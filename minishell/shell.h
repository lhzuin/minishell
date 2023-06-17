// minishell.h
#ifndef SHELL_H
#define SHELL_H
#include <stdbool.h>
#include <termios.h>
#include "jobs.h"

// Your header file content (function prototypes, type definitions, etc.)
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"
#define WORD_MAX_SIZE 40
#define LINE_MAX_SIZE 400
#define MAX_NUM_OF_ARGS 10
#define MAX_NUM_OF_PIPES 10

typedef struct Shell
{
    pid_t shell_pgid;
    struct termios shell_tmodes;
    int shell_terminal;
    bool shell_is_interactive;
    Job *first_job; //list of active jobs
    char *cmd_line; //last input line
} Shell;

Shell *create_shell();
void free_shell(Shell *shell);

#endif // MINISHELL_H