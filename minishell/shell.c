#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shell.h"
#include <stdbool.h>
#include <signal.h>


Shell *create_shell()
{
    Shell *shell = ((Shell *)malloc(sizeof(Shell)));
    shell->first_job = NULL;
    shell->shell_is_interactive = true;
    shell->shell_pgid = 0;
    shell->shell_terminal = 0;
    shell->cmd_line = ((char *)malloc(LINE_MAX_SIZE*sizeof(char)));
    return shell;
}


void free_shell(Shell *shell)
{
    free_jobs(shell->first_job);
    if(shell->cmd_line != NULL)
        free(shell->cmd_line);
    free(shell);
}