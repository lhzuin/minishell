#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "parser.h"
#include <stdbool.h>
#include <signal.h>


void print_welcome()
{
    printf("%s\t _____  _____  _____     ____ ____  \n",MAGENTA);
    printf("\t/ ____|/ ____|/ ____|   |___ \\___ \\ \n");
    printf("\t| |    | (___ | |   ______ _)|___) |\n");
    printf("\t| |     \\___ \\| |  |______|__<|__ < \n");
    printf("\t| |____ ____) | |____    ___)|___) |\n");
    printf("\t\\_____|_____/ \\_____|   |____/____/ \n");
    printf("\n%s************* Welcome to CSC-33 Mini Shell *************%s\n", RED, RESET);
    return;
}

void print_exit()
{
    printf("\n%s********* Thank You for using CSC-33 Mini Shell *********%s\n", RED, RESET);
    return;
}

void allocate_mem(ParsedCmd *parsed_cmd)
{
    parsed_cmd->out = (char *) malloc(sizeof(char)*WORD_MAX_SIZE);
    parsed_cmd->in = (char *) malloc(sizeof(char)*WORD_MAX_SIZE);
    for (int i = 0; i < MAX_NUM_OF_ARGS; i++)
        parsed_cmd->args[i] = NULL;

    if (parsed_cmd->out == NULL || parsed_cmd->in == NULL)
    {
        printf("%sCouldn't allocate memory%s\n",YELLOW, RESET);

        // Free any allocated memory before exiting
        if (parsed_cmd->out != NULL)
        {
            free(parsed_cmd->out);
            parsed_cmd-> out = NULL;
        }
        
        if (parsed_cmd->in != NULL)
        {
            free(parsed_cmd->in);
            parsed_cmd->in = NULL;
        }

        free(parsed_cmd);
        parsed_cmd = NULL;

        exit(EXIT_FAILURE);
    }
    return;

}

void free_mem(ParsedCmd *parsed_cmd)
{
    if(parsed_cmd == NULL)
        return;

    if(parsed_cmd->out != NULL)
    {
        free(parsed_cmd->out);
        parsed_cmd->out = NULL;
    }

    if(parsed_cmd->in != NULL)
    {
        free(parsed_cmd->in);
        parsed_cmd->in = NULL;
    }
    free_args(0, parsed_cmd);
    free(parsed_cmd);
    parsed_cmd = NULL;
    return;
}

void free_args(int i, ParsedCmd *parsed_cmd)
{
    if(i < 0)
    {
        printf("%sCannot access negative index for args%s\n",YELLOW, RESET);
        exit(EXIT_FAILURE);
    }
    if (parsed_cmd == NULL || parsed_cmd->args == NULL)
        return;

    while(i < MAX_NUM_OF_ARGS && parsed_cmd->args[i] != NULL)
    {
        free(parsed_cmd->args[i]);
        parsed_cmd->args[i++] = NULL;
    }
    return;
}


int read_line(ParsedCmd *parsed_cmd, char (*cmd_list)[LINE_MAX_SIZE], char *cmd_line, int *input_fds, int *output_fds)
{
    char *inp;
    char cmd[LINE_MAX_SIZE];
    int pipe_idx;

    printf("\n%sCSC33 -> %s", GREEN, RESET);
    inp = fgets(cmd, sizeof(cmd), stdin);
    strcpy(cmd_line, inp);

    if (inp == NULL)
    {
        print_exit();
        exit(0);
    }
    
    pipe_idx = parse_pipe(cmd, cmd_list, input_fds, output_fds);

    for (int i = 0; i<pipe_idx && i < MAX_NUM_OF_PIPES; i++)
    {
        parse_process(cmd_list[i], parsed_cmd + i);
    }
    return pipe_idx;
}

bool check_exit(char **args)
{
    return strcmp(args[0], "exit") == 0;
}

bool check_jobs(char **args)
{
    return strcmp(args[0], "jobs") == 0;
}

bool check_fg(char **args)
{
    return strcmp(args[0], "fg") == 0;
}

void format_file_name(char **word)
{
    if (word == NULL || *word == NULL)
        return;
    
    char name[WORD_MAX_SIZE];
    snprintf(name, sizeof(name), "./%s", *word);
    strcpy(*word, name);
    return;
} 

int open_input_file(char *file_name)
{
    if (file_name == NULL || strlen(file_name)<=0)
        return STDIN_FILENO;
    format_file_name(&file_name);

    int input_fds = open(file_name, O_RDONLY);
    if (input_fds < 0) 
        printf("%sError opening the input file%s\n",YELLOW, RESET);
    return input_fds;
}

int open_output_file(char *file_name)
{
    if (file_name == NULL || strlen(file_name)<=0)
        return STDOUT_FILENO;
    
    format_file_name(&file_name);

    int output_fds = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (output_fds < 0) 
        printf("%sError opening the output file%s\n",YELLOW, RESET);
    return output_fds;
}

void format_word(char **word)
{
    if (word == NULL || *word == NULL)
        return;
    int len = strlen(*word);
    if (len > 0 && ((*word)[len-1] == '\n' || (*word)[len-1] == ' '))
        (*word)[len-1] = '\0';
    
    if (len > 0 && ((*word)[0] == '\n' || *word[0] == ' '))
        (*word)++;
    return;
} 

void clean_responses(ParsedCmd *parsed_cmd)
{
    if (parsed_cmd->in != NULL)
        parsed_cmd->in[0] = '\0';
    if (parsed_cmd->out != NULL)
        parsed_cmd->out[0] = '\0';
    return;
}

void parse_process(char* cmd, ParsedCmd *parsed_cmd)
{
    //printf("4\n");
    char *word;
    
    // Extracts execution arguments
    if(cmd == NULL)
    {
        free_args(0, parsed_cmd);
        return;
    }
    word = strtok(cmd, " ");
    int i = 0;
    while (i < MAX_NUM_OF_ARGS && word != NULL) {
        format_word(&word);
        if(parsed_cmd->args[i] == NULL)
            parsed_cmd->args[i] = (char *) malloc(sizeof(char)*WORD_MAX_SIZE);
        if(parsed_cmd->args[i] == NULL)
        {
            printf("%sCouldn't allocate memory%s\n",YELLOW, RESET);
            exit(EXIT_FAILURE);
        }
        strcpy(parsed_cmd->args[i++], word);
        word = strtok(NULL, " ");
    }
    free_args(i, parsed_cmd);
    return;
} 


int parse_pipe(char* line, char (*cmd_list)[LINE_MAX_SIZE], int *input_fds, int *output_fds)
{
    char *cmd;
    char *temp;
    char *temp2;
    char *word;

    // Extracts line without output
    temp = strtok(line, ">");

    // Extracts output
    word = strtok(NULL, "\n");
    if (word != NULL)
    {
        format_word(&word);
        *output_fds = open_output_file(word);
    }
    
    // Extracts line without input and output
    temp2 = strtok(temp, "<");

    // Extracts input
    word = strtok(NULL," ");
    if (word != NULL)
    {
        format_word(&word);
        *input_fds = open_input_file(word);
    }
    //printf("temp2: %s\n", temp2);
    //printf("temp: %s\n", temp);
    cmd = strtok(temp2, "|");
    //printf("cmd: %s\n", cmd);
    int i = 0;
    while (i < MAX_NUM_OF_PIPES && cmd != NULL) {
        format_word(&cmd);
        strcpy(cmd_list[i++], cmd);
        //printf("cmd: %s\n", cmd);
        cmd = strtok(NULL, "|");
    }
    //free_args(i, parsed_cmd);
    return i;
} 

