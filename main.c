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
    char cmd_list[MAX_NUM_OF_PIPES][LINE_MAX_SIZE];
    int pipe_idx;
    int output_fds = STDOUT_FILENO;
    int input_fds = STDIN_FILENO;
    Job *new_job = NULL;
    Job *last_job = NULL;
    Shell *shell = create_shell();
    ParsedCmd *parsed_cmd = create_parser();
 
    print_welcome();
    init_shell(shell);
    
    while(true)
    {
        // Cleans previous input and output response
        for (int i = 0; i < MAX_NUM_OF_PIPES; i++)
            clean_responses(parsed_cmd + i);

        // Reads new command line
        pipe_idx = read_line(parsed_cmd, cmd_list, shell->cmd_line, &input_fds, &output_fds);

        if(input_fds < 0 || output_fds < 0 || pipe_idx < 0)
            continue;
        
        // Checks for custom commands
        if(!execute_custom_commands(parsed_cmd, shell))
        {
            // Creates and executes a new job
            create_job(last_job, &new_job, input_fds, output_fds, pipe_idx, parsed_cmd, shell->cmd_line);
            if(last_job == NULL)
            {
                shell->first_job = new_job;
            }
            last_job = new_job;
            bool foreground = true;
            launch_job(new_job, input_fds, output_fds, pipe_idx, foreground, shell);
        }
        
    }
    free_mem(parsed_cmd, shell);
    return 0;
}
