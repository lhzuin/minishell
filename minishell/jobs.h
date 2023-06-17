#ifndef JOBS_H
#define JOBS_H
#include <stdbool.h>
#include <unistd.h>
#include "parser.h"
#include "process.h"
#include <signal.h>
#include <termios.h>


typedef struct Job
{
  struct Job *next;           /* next active job */
  char *command;              /* command line, used for messages */
  Process *first_process;     /* list of processes in this job */
  pid_t pgid;                 /* process group ID */
  bool notified;              /* true if user told about stopped job */
  struct termios tmodes;      /* saved terminal modes */
  int stdin, stdout;  /* standard i/o channels */
  int job_id;
} Job;

void create_job(Job *last_job, Job **new_job, int input_fds, int output_fds, int pipe_idx, ParsedCmd *parsed_cmds, char *cmd_line);
void free_jobs(Job *first_job);
Job *find_job (pid_t pgid, Job *first_job);
Job *find_job_by_job_id (int job_id, Job *first_job);
bool job_is_stopped (Job *j);
bool job_is_completed (Job *j);
void mark_job_as_running (Job *j);
void display_jobs(Job *first_job);
void print_job(Job *job);

#endif