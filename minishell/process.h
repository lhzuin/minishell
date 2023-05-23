#ifndef PROCESS_H
#define PROCESS_H
#include <stdbool.h>
#include <unistd.h>
#include "minishell.h"
//#include "jobs.h"



/* A process is a single process.  */
typedef struct Process
{
  struct Process *next;       /* next process in pipeline */
  char **argv;                /* for exec */
  pid_t pid;                  /* process ID */
  bool completed;             /* true if process has completed */
  bool stopped;               /* true if process has stopped */
  int status;                 /* reported status value */
} Process;

void launch_process(Process *p, int input_fds, int output_fds);
void create_process_linked_list(int pipe_idx, ParsedCmd *parsed_cmds, Process **first_process);
void create_process(Process **last_process, Process **new_process, char **args);
void free_processes(Process *first_process);

#endif