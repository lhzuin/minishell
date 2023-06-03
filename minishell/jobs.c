#include "jobs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

void create_job(Job *last_job, Job **new_job, int input_fds, int output_fds, int pipe_idx, ParsedCmd *parsed_cmds, char *cmd_line)
{//talvez alterar para Job **last_job
    //printf("0\n");
    //printf("parsed_cmd[0]: %s\n", parsed_cmds->args[0]);
    //printf("parsed_cmd[1]: %s\n", (parsed_cmds + 1)->args[0]);
    *new_job = ((Job *)malloc(sizeof(Job)));

    (*new_job)->stdin = input_fds;
    (*new_job)->stdout = output_fds;
    (*new_job)->pgid = last_job->pgid + 1;//0;//last_job->pgid + 1; //0; //
    (*new_job)->first_process = NULL;
    (*new_job)->command = ((char *)malloc(LINE_MAX_SIZE*sizeof(char)));
    strcpy((*new_job)->command, cmd_line);

    last_job->next = *new_job;

    create_process_linked_list(pipe_idx, parsed_cmds, &(*new_job)->first_process);
    //for(Process *p = (*new_job)->first_process; p != NULL; p = p->next)
        //printf("p: %s\n", p->argv[0]);
    //printf("1\n");
    
}


void free_jobs(Job *first_job)
{
    Job *job = first_job;
    Job *last_job = NULL;

    for(; job != NULL; )
    {
        last_job = job;
        job = job->next;
        free_processes(last_job->first_process);
        free(last_job);
    }
}

/* Find the active job with the indicated pgid.  */
Job *find_job (pid_t pgid, Job *first_job)
{
  Job *j;
  for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  printf("F\n");
  fflush(stdout);
  return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
bool job_is_stopped (Job *j)
{
  Process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return false;
  return true;
}

/* Return true if all processes in the job have completed.  */
bool job_is_completed (Job *j)
{
  Process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return false;
  return true;
}



/* Mark a stopped job J as being running again.  */

void mark_job_as_running (Job *j)
{
    Process *p;

    for (p = j->first_process; p; p = p->next)
        p->stopped = false;
    j->notified = false;
}


/* Format information about job status for the user to look at.  */

void format_job_info (Job *j, const char *status)
{
  fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}


void display_jobs(Job *first_job)
{
  for (Job *j = first_job; j != NULL; j = j->next)
  {
    if (!job_is_completed(j) && job_is_stopped(j))
      printf ("[%ld]\tStopped\t\t%s\n", (long)j->pgid, j->command);
  }
}