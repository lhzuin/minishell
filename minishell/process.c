#include "process.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


void create_process(Process **last_process, Process **new_process, char **args)
{
    (*new_process) = ((Process *)malloc(sizeof(Process)));
    (*new_process)->argv = args;
    (*new_process)->next = NULL;
    if ((*last_process) != NULL)
        (*last_process)->next = (*new_process);
    *last_process = *new_process;
}   


void create_process_linked_list(int pipe_idx, ParsedCmd *parsed_cmds, Process **first_process)
{
    ParsedCmd *parsed_cmd = parsed_cmds;
    Process *last_process = NULL;

    for(int i = 0; i<pipe_idx; i++)
    {
        parsed_cmd = parsed_cmds + i;
        create_process(&last_process, first_process, parsed_cmd->args);
        if(i == 0) // if this is the first process
            *first_process = last_process; // point first_process to it
        first_process = &((*first_process)->next); // shift the first_process pointer
    }
}


void launch_process(Process *p, int input_fds, int output_fds)
{
    //printf("11\n");
    //printf("p->argv[0]: %s", p->argv[0]);
    if (input_fds < 0)
    {
        printf("%sError: input fd cannot be negative%s\n",YELLOW, RESET);
        exit(1);
    }
    if (output_fds < 0)
    {
        printf("%sError: output fd cannot be negative%s\n",YELLOW, RESET);
        exit(1);
    }
        
    if (input_fds != STDIN_FILENO && dup2(input_fds, STDIN_FILENO) < 0)
    {
        printf("%sError redirecting stdin%s\n",YELLOW, RESET);
        close(input_fds);
        exit(-1);
    }
    //printf("17");
    if (output_fds != STDOUT_FILENO && dup2(output_fds, STDOUT_FILENO) < 0)
    {
        printf("%sError redirecting stdout%s\n",YELLOW, RESET);
        close(output_fds);
        exit(-1);
    }
    //printf("12\n");
    // Execute command
    execv(p->argv[0], p->argv);

    // If execv fails
    printf("%sDidn't recognize command%s",YELLOW, RESET);
    exit(1);
}


void free_processes(Process *first_process)
{
    Process *p = first_process;
    Process *last_p = NULL;

    for(; p != NULL; )
    {
        last_p = p;
        p = p->next;
        free(last_p);
    }
}