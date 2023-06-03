//#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shell.h"
#include <stdbool.h>
//#include "process.h"
#include <signal.h>


Shell *create_shell()
{
    Job *first_job = ((Job *)malloc(sizeof(Job)));
    Shell *shell = ((Shell *)malloc(sizeof(Shell)));
    first_job->first_process = NULL;
    first_job->pgid = 0;
    shell->first_job = first_job;
    shell->shell_is_interactive = true;
    shell->shell_pgid = 0;
    shell->shell_terminal = 0; //?
    return shell;
}


void free_shell(Shell *shell)
{
    free_jobs(shell->first_job);
    free(shell);
}