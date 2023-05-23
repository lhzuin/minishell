#include "jobs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

void create_job(Job *last_job, Job **new_job, int input_fds, int output_fds, int pipe_idx, ParsedCmd *parsed_cmds)
{
    //printf("0\n");
    //printf("parsed_cmd[0]: %s\n", parsed_cmds->args[0]);
    //printf("parsed_cmd[1]: %s\n", (parsed_cmds + 1)->args[0]);
    *new_job = ((Job *)malloc(sizeof(Job)));

    (*new_job)->stdin = input_fds;
    (*new_job)->stdout = output_fds;
    (*new_job)->pgid = last_job->pgid + 1;
    (*new_job)->first_process = NULL;

    last_job->next = *new_job;

    create_process_linked_list(pipe_idx, parsed_cmds, &(*new_job)->first_process);
    //for(Process *p = (*new_job)->first_process; p != NULL; p = p->next)
        //printf("p: %s\n", p->argv[0]);
    //printf("1\n");
    
}

void launch_job(Job *new_job, int input_fds, int output_fds, int pipe_idx)
{
    //printf("2\n");
    Process *p = new_job->first_process; 
    int pipefds[2];
    int pid;

    for (int i = 0; i < pipe_idx; i++, p = p->next)
    {
        //printf("5\n");
        if (check_exit(p->argv))
        {
            print_exit();
            exit(0); //alterar para incluir free
        }

        // Setup pipes
        //printf("6\n");
        if (i < pipe_idx -1)
        {
            if (pipe(pipefds) < 0)
                exit(-1);
            
            output_fds = pipefds[1];
            //printf("7\n");
        }
        else
        {
            //printf("8\n");
            output_fds = new_job->stdout;
        }

        // Creates parallel process
        pid = fork();
        if (pid == 0) // Child process
        {
            //printf("9\n");
            //printf("input_fds: %d\n", input_fds);
            //printf("output_fds: %d\n", output_fds);
            launch_process(p, input_fds, output_fds);
        }
        else if (pid < 0)
        {
            printf("%sFailed to do fork%s\n",YELLOW, RESET);
            exit(-1);
        }
        else
        {
            //printf("10\n");
            // Waits for Child Process to finish
            p->pid = pid;
            //wait(NULL);
        }
        
        if (input_fds != new_job->stdin && input_fds != STDIN_FILENO)
            close(input_fds);

        if (output_fds != new_job->stdout && output_fds != STDOUT_FILENO)
            close(output_fds);
        input_fds = pipefds[0];

    }
    // Waits for all child processes to finish
    int status;
    while ((pid = wait(&status)) != -1)
        continue;
        //printf("pid: %d", pid);
    //wait(NULL); 
    if (new_job->stdin != STDIN_FILENO)
        close(new_job->stdin);
    if(new_job->stdout != STDOUT_FILENO)
        close(new_job->stdout);
    //printf("3\n");
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