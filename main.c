#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include "minishell/minishell.h"
#include "minishell/jobs.h"


int main()
{
    ParsedCmd *parsed_cmd = ((ParsedCmd *)malloc(MAX_NUM_OF_PIPES*sizeof(ParsedCmd)));
    Job *new_job = NULL;
    Job *first_job = ((Job *)malloc(sizeof(Job)));
    Job *last_job = first_job;
    first_job->first_process = NULL;
    first_job->pgid = 0;
    int output_fds = STDOUT_FILENO;
    int input_fds = STDIN_FILENO;


    char cmd_list[MAX_NUM_OF_PIPES][LINE_MAX_SIZE];
    int pipe_idx;
    
    print_welcome();
    
    // Allocate memory for variables
    for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
        allocate_mem(parsed_cmd + i);

    while(true)
    {
        // Cleans previous input and output response
        for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
            clean_responses(parsed_cmd + i); //alterar para incluir os varios cmds

        // Reads new command line
        pipe_idx = read_line(parsed_cmd, cmd_list, &input_fds, &output_fds);

        if(input_fds < 0 || output_fds < 0)
            continue;

        create_job(last_job, &new_job, input_fds, output_fds, pipe_idx, parsed_cmd);
        last_job = new_job;
        launch_job(new_job, input_fds, output_fds, pipe_idx);
        
    }
    for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
        free_mem(parsed_cmd + i);
    free_jobs(first_job);
    return 0;
}
