// utils.h
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <termios.h>
#include "shell.h"
#include "jobs.h"
#include "process.h"

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


void init_shell (Shell *shell);
void launch_job(Job *new_job, int input_fds, int output_fds, int pipe_idx, bool foreground, Shell *shell);
void launch_process(Process *p, pid_t pgid, int input_fds, int output_fds, bool foreground, Shell *shell);
void put_job_in_foreground (Job *j, Shell *shell, bool cont);
void put_job_in_background (Job *j, bool cont);
void continue_job (Job *j, Shell *shell, bool foreground);
int mark_process_status (pid_t pid, int status, Shell *shell);
void update_status (Shell *shell);
void wait_for_job (Job *j, Shell *shell);
void do_job_notification (Job *first_job, Shell *shell);
bool execute_custom_commands(ParsedCmd *parsed_cmd, Shell *shell);
void free_mem(ParsedCmd *parsed_cmd, Shell *shell);



#endif // UTILS_H