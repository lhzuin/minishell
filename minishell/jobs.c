#include "jobs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

void create_job(Job *last_job, Job **new_job, int input_fds, int output_fds, int pipe_idx, ParsedCmd *parsed_cmds, char *cmd_line)
{
    *new_job = ((Job *)malloc(sizeof(Job)));

    (*new_job)->stdin = input_fds;
    (*new_job)->stdout = output_fds;
    (*new_job)->pgid = 0;
    if (last_job == NULL)
    {
      (*new_job)->job_id = 1;
    }
    else
    {
      last_job->next = *new_job;
      (*new_job)->job_id = last_job->job_id + 1;
    }
    
    (*new_job)->first_process = NULL;
    (*new_job)->command = ((char *)malloc(LINE_MAX_SIZE*sizeof(char)));
    strcpy((*new_job)->command, cmd_line);   

    create_process_linked_list(pipe_idx, parsed_cmds, &(*new_job)->first_process);    
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
        if(last_job->command != NULL)
          free(last_job->command);
        free(last_job);
    }
}

/* Find the active job with the indicated pgid.  */
Job *find_job (pid_t pgid, Job *first_job)
{
  if(first_job == NULL)
    return NULL;
  Job *j;
  for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
}

/* Find the active job with the indicated job_id.  */
Job *find_job_by_job_id (int job_id, Job *first_job)
{
  if(first_job == NULL)
    return NULL;
  Job *j;
  for (j = first_job; j; j = j->next)
    if (j->job_id == job_id)
      return j;
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

void display_jobs(Job *first_job)
{
  for (Job *j = first_job; j != NULL; j = j->next)
  {
    if (!job_is_completed(j) && job_is_stopped(j))
      printf ("[%d] %ld\tStopped\t\t%s\n", j->job_id, (long)j->pgid, j->command);
  }
}


void print_job(Job *job)
{
  Process *p = job->first_process;
  printf("Jobs: \ncommand: %s\npgid: %ld\njob_id: %ld\nnotified: %d\n", job->command, (long)job->pgid, (long)job->job_id, job->notified);
  fflush(stdout);

  while(p!= NULL)
  {
    print_process(p);
    p = p->next;
  }


}