// parser.h
#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <termios.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"
#define WORD_MAX_SIZE 40
#define LINE_MAX_SIZE 400
#define MAX_NUM_OF_ARGS 10
#define MAX_NUM_OF_PIPES 10

typedef struct ParsedCmd
{
    char *in;
    char *out;
    char *args[MAX_NUM_OF_ARGS];
} ParsedCmd;

void print_welcome();
void print_exit();
void print_fg_error();
void allocate_mem(ParsedCmd *parsed_cmd);
void free_parser_mem(ParsedCmd *parsed_cmd);
void free_args(int i, ParsedCmd *parsed_cmd);
int read_line(ParsedCmd *parsed_cmd, char (*cmd_list)[LINE_MAX_SIZE], char *cmd_line, int *input_fds, int *output_fds);
void read_cmd(ParsedCmd *parsed_cmd);
bool check_exit(ParsedCmd *parsed_cmd);
bool check_jobs(ParsedCmd *parsed_cmd);
bool check_fg(ParsedCmd *parsed_cmd);
void format_word(char **word);
void clean_responses(ParsedCmd *parsed_cmd);
void parse_process(char* cmd, ParsedCmd *parsed_cmd);
int parse_pipe(char* line, char (*cmd_list)[LINE_MAX_SIZE], int *input_fds, int *output_fds);
ParsedCmd *create_parser();

#endif // PARSER_H
