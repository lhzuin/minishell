#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include "minishell/shell.h"
#include "minishell/jobs.h"
#include "minishell/parser.h"
#include "minishell/utils.h"



int main()
{
    ParsedCmd *parsed_cmd = ((ParsedCmd *)malloc(MAX_NUM_OF_PIPES*sizeof(ParsedCmd)));
    //Shell *shell = ((Shell *)malloc(sizeof(Shell)));
    Job *new_job = NULL;
    //Job *first_job = ((Job *)malloc(sizeof(Job)));
    //Job *last_job = first_job;
    //first_job->first_process = NULL;
    //first_job->pgid = 0;
    int output_fds = STDOUT_FILENO;
    int input_fds = STDIN_FILENO;

    Shell *shell = create_shell();
    Job *last_job = shell->first_job;


    char cmd_list[MAX_NUM_OF_PIPES][LINE_MAX_SIZE];
    char *cmd_line = ((char *)malloc(LINE_MAX_SIZE*sizeof(char)));
    int pipe_idx;
    
    print_welcome();
    init_shell(shell);
    
    // Allocate memory for variables
    for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
        allocate_mem(parsed_cmd + i);

    while(true)
    {
        // Cleans previous input and output response
        for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
            clean_responses(parsed_cmd + i); //alterar para incluir os varios cmds

        // Reads new command line
        pipe_idx = read_line(parsed_cmd, cmd_list, cmd_line, &input_fds, &output_fds); // começa em 1

        if(input_fds < 0 || output_fds < 0)
            continue;
        create_job(last_job, &new_job, input_fds, output_fds, pipe_idx, parsed_cmd, cmd_line);
        last_job = new_job;
        bool foreground = true;
        launch_job(new_job, input_fds, output_fds, pipe_idx, foreground, shell);
        
    }
    for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
        free_mem(parsed_cmd + i);
    //free_jobs(shell->first_job);
    free_shell(shell);
    free(cmd_line);
    return 0;
}
