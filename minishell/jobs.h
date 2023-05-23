#ifndef JOBS_H
#define JOBS_H
#include <stdbool.h>
#include <unistd.h>
#include "minishell.h"
#include "process.h"


typedef struct Job
{
  struct Job *next;           /* next active job */
  char *command;              /* command line, used for messages */
  Process *first_process;     /* list of processes in this job */
  pid_t pgid;                 /* process group ID */
  bool notified;              /* true if user told about stopped job */
  //struct termios tmodes;      /* saved terminal modes */
  int stdin, stdout;  /* standard i/o channels */
} Job;

void create_job(Job *last_job, Job **new_job, int input_fds, int output_fds, int pipe_idx, ParsedCmd *parsed_cmds);
void launch_job(Job *new_job, int input_fds, int output_fds, int pipe_idx);
void free_jobs(Job *first_job);

#endif