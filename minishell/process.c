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

void print_process(Process* p)
{
  printf("Process: \nstatus: %d\npid: %ld\ncompleted: %d\nstopped: %d\n", p->status, (long)p->pid, p->completed, p->stopped);

}