#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        return 1;
    }

    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    
    if (pipe(pipefd1) == -1)
    {
        perror("pipe");
        return 1;
    }

    
    if (pipe(pipefd2) == -1)
    {
        perror("pipe");
        return 1;
    }

    
    pid1 = fork();
    if (pid1 == 0)
    {
   
        pid2 = fork();

        if (pid2 == 0)
        {
            // Replace standard input with the read end of the second pipe
            dup2(pipefd2[0], 0);
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            execvp(sort_args[0], sort_args);
        }
        else
        {
            // Replace standard input with the read end of the first pipe
            dup2(pipefd1[0], 0);
            // Replace standard output with the write end of the second pipe
            dup2(pipefd2[1], 1);

            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            // Execute grep
            execvp(grep_args[0], grep_args);
        }
    }
    else
    {
        // Replace standard output with the write end of the first pipe
        dup2(pipefd1[1], 1);
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execvp(cat_args[0], cat_args);
    }

    
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
