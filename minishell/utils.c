#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
//#include "process.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include "utils.h"




/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

void init_shell (Shell *shell)
{

    /* See if we are running interactively.  */
    shell->shell_terminal = STDIN_FILENO;
    shell->shell_is_interactive = isatty (shell->shell_terminal);

    if (shell->shell_is_interactive)
    {
        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (shell->shell_terminal) != (shell->shell_pgid = getpgrp ()))
        kill (- shell->shell_pgid, SIGTTIN);

        /* Ignore interactive and job-control signals.  */
        signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_DFL);

        /* Put ourselves in our own process group.  */
        shell->shell_pgid = getpid ();
        shell->first_job->pgid = shell->shell_pgid; //teste
        if (setpgid (shell->shell_pgid, shell->shell_pgid) < 0)
        {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp (shell->shell_terminal, shell->shell_pgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr (shell->shell_terminal, &(shell->shell_tmodes));
    }
}

void launch_job(Job *new_job, int input_fds, int output_fds, int pipe_idx, bool foreground, Shell *shell)
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
        else if (check_jobs(p->argv))
        {
            display_jobs(shell->first_job);
            return;
            //break;
        }
        else if (check_fg(p->argv)) // checar se faz algum sentido
        {
            pid_t pgid = (pid_t) atoi(p->argv[1]);
            printf("%ld\n", (long)pgid);
            Job *j = find_job(pgid, shell->first_job);
            printf("%ld\n", (long)j->pgid);
            fflush(stdout);
            continue_job(j, shell, true);
            return;
            //break;
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
            launch_process(p, new_job->pgid, input_fds, output_fds, foreground, shell);
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
            if(shell->shell_is_interactive)
            {
                if(!new_job->pgid)
                    new_job->pgid = pid;
                setpgid(pid, new_job->pgid);
            }
            //wait(NULL);
        }
        
        if (input_fds != new_job->stdin && input_fds != STDIN_FILENO)
            close(input_fds);

        if (output_fds != new_job->stdout && output_fds != STDOUT_FILENO)
            close(output_fds);
        input_fds = pipefds[0];

    }
    format_job_info (new_job, "launched");

    if (!shell->shell_is_interactive)
        wait_for_job(new_job, shell);
    else if (foreground)
        put_job_in_foreground(new_job, shell, false);
    else
        put_job_in_background(new_job, false);
    // Waits for all child processes to finish
    //int status;
    //while ((pid = wait(&status)) != -1)
        //continue;
        //printf("pid: %d", pid);
    //wait(NULL); 
    if (new_job->stdin != STDIN_FILENO)
        close(new_job->stdin);
    if(new_job->stdout != STDOUT_FILENO)
        close(new_job->stdout);
    //printf("3\n");
}

void launch_process(Process *p, pid_t pgid, int input_fds, int output_fds, bool foreground, Shell *shell)
{
    //printf("11\n");
    //printf("p->argv[0]: %s", p->argv[0]);
    pid_t pid;
    
    if (input_fds < 0)
    {
        printf("%sError: input fd cannot be negative%s\n",YELLOW, RESET);
        exit(1);
    }
    if (output_fds < 0)
    {
        printf("%sError: output fd cannot be negative%s\n",YELLOW, RESET);
        exit(1);
    }

    if (shell->shell_is_interactive)
    {
        /* Put the process into the process group and give the process group
            the terminal, if appropriate.
            This has to be done both by the shell and in the individual
            child processes because of potential race conditions.  */
        pid = getpid ();
        if (pgid == 0) 
            pgid = pid;
        setpgid (pid, pgid);
        if (foreground)
            tcsetpgrp (shell->shell_terminal, pgid);

        /* Set the handling for job control signals back to the default.  */
        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }
        
    if (input_fds != STDIN_FILENO && dup2(input_fds, STDIN_FILENO) < 0)
    {
        printf("%sError redirecting stdin%s\n",YELLOW, RESET);
        close(input_fds);
        exit(-1);
    }
    //printf("17");
    if (output_fds != STDOUT_FILENO && dup2(output_fds, STDOUT_FILENO) < 0)
    {
        printf("%sError redirecting stdout%s\n",YELLOW, RESET);
        close(output_fds);
        exit(-1);
    }
    //printf("12\n");
    // Execute command
    execv(p->argv[0], p->argv);

    // If execv fails
    printf("%sDidn't recognize command%s",YELLOW, RESET);
    exit(1);
}


/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */

void put_job_in_foreground (Job *j, Shell *shell, bool cont)
{
    /* Put the job into the foreground.  */
    //tcsetpgrp (shell_terminal, j->pgid);


    /* Send the job a continue signal, if necessary.  */
    if (cont)
    {   
        printf("shell_terminal: %d\n", shell->shell_terminal);
        printf("shell_pgid: %ld\n", (long)shell->shell_pgid);
        tcsetattr (shell->shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill (- j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
        
        //if (kill (- j->first_process->pid, SIGCONT) < 0)
            //perror ("kill (SIGCONT)");
    }

    /* Wait for it to report.  */
    wait_for_job (j, shell);

    /* Put the shell back in the foreground.  */
    tcsetpgrp (shell->shell_terminal, shell->shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcgetattr (shell->shell_terminal, &j->tmodes);
    tcsetattr (shell->shell_terminal, TCSADRAIN, &(shell->shell_tmodes));
}


/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */

void put_job_in_background (Job *j, bool cont)
{
  /* Send the job a continue signal, if necessary.  */
  if (cont)
    if (kill (-j->pgid, SIGCONT) < 0)
      perror ("kill (SIGCONT)");
}


/* Continue the job J.  */

void continue_job (Job *j, Shell *shell, bool foreground)
{
    if(j == NULL)
        return;
    mark_job_as_running (j);
    if (foreground)
        put_job_in_foreground (j, shell, true);
    else
        put_job_in_background (j, true);
}

/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */

int mark_process_status (pid_t pid, int status, Shell *shell)
{
    Job *j;
    Process *p;
    printf("mark_process: %ld\n", (long) pid);

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for (j = shell->first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid)
                {
                    p->status = status;
                    if (WIFSTOPPED (status))
                    {
                        printf("Stopped\n");
                        p->stopped = true;
                    }
                        
                    else
                    {
                        printf("Completed\n");
                        p->completed = true;
                        if (WIFSIGNALED (status))
                        fprintf (stderr, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
                    }
                    return 0;
                }
        fprintf (stderr, "No child process %d.\n", pid);
        return -1;
    }

    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report.  */
        return -1;

    /* Other weird errors.  */
    perror ("waitpid");
    return -1;
    
}

/* Check for processes that have status information available,
   without blocking.  */

void update_status (Shell *shell)
{
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
    while (!mark_process_status (pid, status, shell));
}

/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void wait_for_job (Job *j, Shell *shell)
{
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status (pid, status, shell) && !job_is_stopped (j)&& !job_is_completed (j));
}


/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */

void do_job_notification (Job *first_job, Shell *shell)
{
    Job *j, *jlast, *jnext;

    /* Update status information for child processes.  */
    update_status(shell);

    jlast = NULL;
    for (j = first_job; j; j = jnext)
    {
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
            completed and delete it from the list of active jobs.  */
        if (job_is_completed (j)) {
            format_job_info (j, "completed");
            if (jlast)
                jlast->next = jnext;
            else
                first_job = jnext;
            jnext = NULL;
            free_jobs(j);
        }

        /* Notify the user about stopped jobs,
            marking them so that we won’t do this more than once.  */
        else if (job_is_stopped (j) && !j->notified) {
            format_job_info (j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        /* Don’t say anything about jobs that are still running.  */
        else
            jlast = j;
    }
}
