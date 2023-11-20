
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2]; // Pipe from P1 to P2
    int fd2[2]; // Pipe from P2 to P1

    char fixed_str[] = "howard.edu";
    char input_str[100];

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("P1: Enter a string to send to P2: ");
    scanf("%s", input_str);

    pid_t p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }
    else if (p > 0) // P1
    {
        close(fd1[0]); // Close reading end of P1 to P2 pipe
        close(fd2[1]); // Close writing end of P2 to P1 pipe

        // Write input string to P2
        write(fd1[1], input_str, strlen(input_str) + 1);

        // Wait for P2 to send concatenated string
        char concat_str[100];
        read(fd2[0], concat_str, 100);

        // Print the concatenated string received from P2
        printf("P1: Concatenated string from P2: %s\n", concat_str);

        // Now P1 waits for P2 to send the second input string
        read(fd1[0], input_str, 100);

        // Print the second input received from P2
        printf("P1: Second input from P2: %s\n", input_str);

        close(fd1[1]); // Close writing end of P1 to P2 pipe
        close(fd2[0]); // Close reading end of P2 to P1 pipe
    }
    else // P2
    {
        close(fd1[1]); // Close writing end of P1 to P2 pipe
        close(fd2[0]); // Close reading end of P2 to P1 pipe

        // Read the input string from P1
        read(fd1[0], input_str, 100);

        // Concatenate the fixed string with the received string
        strcat(input_str, fixed_str);

        // Print the concatenated string
        printf("P2: Concatenated string: %s\n", input_str);

        // Send the concatenated string back to P1
        write(fd2[1], input_str, strlen(input_str) + 1);

        // Prompt user for a second input
        printf("P2: Enter a second string to send to P1: ");
        scanf("%s", input_str);

        // Send the second input string to P1
        write(fd1[1], input_str, strlen(input_str) + 1);

        close(fd1[0]); // Close reading end of P1 to P2 pipe
        close(fd2[1]); // Close writing end of P2 to P1 pipe
    }

    return 0;
}
