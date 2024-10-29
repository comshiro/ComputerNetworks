
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_TO_SERVER "FIFO_REC"
#define FIFO_FROM_SERVER "FIFO_SEND"

int char_to_int(char *s)
{
    int x = 0;
    ;
    for (size_t i = 0; i < strlen(s); i++)
    {
        x = x * 10 + (s[i] - '0');
    }
    return x;
}

int main()
{
    int quit = 1;
    char input[300];
    char response[300];

    /// DESCHIDEM FIFO-URILE
    printf("Connecting to server...\n");
    int fd_write = open("/tmp/" FIFO_TO_SERVER, O_WRONLY);
    if (fd_write == -1)
    {
        perror("Failed to open FIFO for writing");
        exit(1);
    }

    int fd_read = open("/tmp/" FIFO_FROM_SERVER, O_RDONLY);
    if (fd_read == -1)
    {
        perror("Failed to open FIFO for reading response");
        exit(1);
    }
    printf("Enter a message to send to the server: ");

    /// INCEPEM LOOP-UL DE SCRIERE CITIRE
    while (quit)
    {
        printf("Enter command: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("Error reading input");
        } /// CITIM COMANDA DE LA TASTATURA

        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0)
        {
            printf("Quiting...");
            quit = 0;
            break;
        }

        if (write(fd_write, input, strlen(input)) == -1) /// CLIENTUL SCRIE COMANDA IN FIFO
        {
            perror("Error writing to the FIFO");
            exit(1);
        }

        printf("Message sent to server: \"%s\"\n", input);

        char res_size[5];
        int result_size = 0;
        int read_bytes;

        read_bytes = read(fd_read, &result_size, sizeof(int));
        printf("Read %d bytes\n", read_bytes);
        if (read_bytes == -1)
            perror("Error reading response from FIFO");
        else
        /// CITIRE RESULTAT TRIMIS DE SERVER
        {
            res_size[read_bytes] = '\0';

            if ((read(fd_read, response, result_size)) == -1)
                perror("Eroare citire rezultat de marimea specificata!");
            else
            {
                response[result_size] = '\0';
                printf("Server's response: \"%s\"\n", response);
            }
        }

    }
    close(fd_read);
    close(fd_write);
}
