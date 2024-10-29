#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <utmp.h>
#include <pwd.h>
#include <sys/mman.h>
#include <ctype.h>

#define FIFO_FROM_CLIENT "FIFO_REC"
#define FIFO_TO_CLIENT "FIFO_SEND"
#define CONFIGURATION_FILE "USERS.txt"
#define BUF_SIZE 1024

int *logged_in; //Flag for knowing if the user is logged in
struct FIFOs
{
    const char *fifo_from_client;
    const char *fifo_to_client;
};

struct FIFOs createFIFO()
{
    const char *fifo_from_client = "/tmp/" FIFO_FROM_CLIENT;
    const char *fifo_to_client = "/tmp/" FIFO_TO_CLIENT;

    remove(fifo_from_client);
    remove(fifo_to_client);

    if (mkfifo(fifo_from_client, 0666) == -1)
    {
        perror("mkfifo failed for client-to-server");
    }

    if (mkfifo(fifo_to_client, 0666) == -1)
    {
        perror("mkfifo failed for server-to-client");
    }

    struct FIFOs fifos = {fifo_from_client, fifo_to_client};
    return fifos;
}


size_t setupFIFOsOpenRead(struct FIFOs *fifos)
{
    int fd_read = open(fifos->fifo_from_client, O_RDONLY);
    if (fd_read == -1)
    {
        perror("open read failed");
        exit(1);
    }
    return fd_read;
}

size_t setupFIFOsOpenWrite(struct FIFOs *fifos)
{
    int fd_read = open(fifos->fifo_to_client, O_WRONLY);
    if (fd_read == -1)
    {
        perror("open read failed");
        exit(1);
    }
    return fd_read;
}

int main()
{
    int *logged_in = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //The flag is mapped to be visible to both parent and child
    *logged_in = 0;
    if (logged_in == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }

    printf("Creating Server..\n");
    struct FIFOs fifos = createFIFO();
    printf("Open Read\n");
    int fd_read = setupFIFOsOpenRead(&fifos);
    printf("Open Write\n");
    int fd_write = setupFIFOsOpenWrite(&fifos);

    int p_p2c[2]; /// Pipe for parent -> child communication
    int s_c2p[2]; // Socketpair for child -> parent communication

    printf("Starting Server..\n");
    while (13)
    {
        if (pipe(p_p2c) == -1)
        {
            perror("pipe failed");
        }
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, s_c2p) == -1)
        {
            perror("socketpair failed");
        }
        // The server(parent) is reading from the fifo
        char buf[BUF_SIZE];
        int bytes_read = 0;
        while (bytes_read == 0)
        {
            bytes_read = read(fd_read, buf, BUF_SIZE - 1);
        }

        if (bytes_read == -1)
        {
            perror("read failed");
            continue;
        }
        buf[bytes_read] = '\0';
        printf("Server got the command:%s\n  (print used for debugging) ", buf);
        /**
         * Getting a command implies:
         *  (Child-process)
         *  1. Create child w fork
         *  2. Parse command in child 
         *  3. Execute command in child 
         *  4. Write size + command result on the child pipe
         * (Parent Process)
         *  1. Write the command on the pipe for the child
         *  2. Wait for the result from the child
         *  3. Continue execution
         **/
        int pid = fork();
        switch (pid)
        {
        case -1:
            perror("Eroare la fork");
            break;
        case 0: // Child process
            close(p_p2c[1]);
            int num;
            char s[300]; // the command received
            char *result; 

            do 
            {
                if ((num = read(p_p2c[0], s, 300)) == -1)
                    perror("Error reading from parent -> child pipe");
                else
                {
                    s[num] = '\0';
                }
            } while (num < 0);

            char *p = strtok(s, " "); ///Parsing the command
            while (p)
            {
                if (strcmp(p, "login") == 0)
                {
                    p = strtok(NULL, " ");
                    printf("%s\n", p);

                    if (*logged_in == 1)
                    {
                        result = "User already logged in";
                    }
                    else
                    {
                        FILE *file = fopen(CONFIGURATION_FILE, "r");
                        if (!file)
                        {
                            perror("Configuration file cannot be opened");
                        }
                        else
                        {
                            char line[256];
                            int user_found = 0;

                            while (fgets(line, sizeof(line), file))
                            {
                                line[strchr(line, '\n') - line] = '\0';
                                if (strcmp(p, line) == 0)
                                {
                                    *logged_in = 1;
                                    result = "Valid user. User has logged in";
                                    user_found = 1;
                                    break;          
                                }
                            }

                            if (!user_found)
                            {
                                *logged_in = 0; 
                                result = "Invalid user. Try again.";
                            }

                            fclose(file);
                        }
                    }
                }

                if (strcmp(p, "get-logged-users") == 0)
                {

                    if (*logged_in == 0)
                    {
                        result = "User is not logged_in";
                    }
                    else
                    {
                        // Start getting logged users information
                        struct utmp *entry;
                        setutent(); // Rewind the utmp file to the start

                        char logged_users_info[4096] = ""; 

                        while ((entry = getutent()) != NULL)
                        {
                            if (entry->ut_type == USER_PROCESS)
                            {
                                char buffer[512];
                                snprintf(buffer, sizeof(buffer), "Username: %s, Host: %s, Login time: %d\n",
                                         entry->ut_user, entry->ut_host, entry->ut_tv.tv_sec); 
                                strcat(logged_users_info, buffer);         
                            }
                        }
                        endutent(); // Close the utmp file

                        if (strlen(logged_users_info) == 0)
                        {
                            result = "No users currently logged in.";
                        }
                        else
                        {
                            result = logged_users_info;
                        }
                    }
                }

                  if (strcmp(p, "get-proc-info") == 0)
                {
                    if (*logged_in == 0)
                    {
                        result = "User is not authenticated";
                    }
                    else
                    {
                        // Get the process info
                        p = strtok(NULL, " ");
                        if (p)
                        {
                            char proc_path[256];
                            snprintf(proc_path, sizeof(proc_path), "/proc/%s/status", p);

                            FILE *proc_file = fopen(proc_path, "r");
                            if (!proc_file)
                            {
                                result = "Failed to open process status file";
                            }
                            else
                            {
                                char proc_info[1024] = "";
                                char line[256];
                                while (fgets(line, sizeof(line), proc_file))
                                {
                                    if (strncmp(line, "Name:", 5) == 0 ||
                                        strncmp(line, "State:", 6) == 0 ||
                                        strncmp(line, "PPid:", 5) == 0 ||
                                        strncmp(line, "Uid:", 4) == 0 ||
                                        strncmp(line, "VmSize:", 7) == 0)
                                    {
                                        strcat(proc_info, line);
                                    }
                                }
                                fclose(proc_file);

                                if (strlen(proc_info) == 0)
                                {
                                    result = "Process info not found";
                                }
                                else
                                {
                                    result = proc_info;
                                }
                            }
                        }
                        else
                        {
                            result = "Invalid PID specified";
                        }
                    }
                }

                if (strcmp(p, "logout") == 0)
                {
                    if (*logged_in == 1)
                    {
                        *logged_in = 0;
                        result = "User logged out.";
                    }
                    else
                    {
                        result = "No user logged in.";
                    }
                }
                p = strtok(NULL, " ");
            }

            /// The child writes the result into the socketpair
            int resultSz = strlen(result) + 1;
            write(s_c2p[1], &resultSz, sizeof(int));
            write(s_c2p[1], result, resultSz);
            printf("Result: %s\n", result);

            close(p_p2c[0]); // Close read end of the pipe
            close(s_c2p[1]); // Close write end of socketpair
            exit(0);         // Exit child process

        default:

            if (write(p_p2c[1], buf, strlen(buf)) == -1) //Writing the command from client to the pipe
            {
                perror("Eroare la 'write' din pipe parent -> child");
            }
            int n_buf[1];
            bytes_read = read(s_c2p[0], n_buf, sizeof(int));//Reading the size of the result sent by the child
            if (bytes_read == -1)
            {
                perror("Eroare la citirea din pipe child -> parent");
            }
            char buf[BUF_SIZE];
            bytes_read = read(s_c2p[0], buf, *n_buf+1); 
            if (bytes_read == -1)
            {
                perror("Eroare la citirea din pipe child -> parent");
            }
            buf[bytes_read] = '\0';
            printf("We got the result: [%s]\n", buf);
            wait(NULL);
            // Send to the client
            printf("Sending to client.\n");

            write(fd_write, &bytes_read, sizeof(int));
            write(fd_write, &buf, bytes_read);
            break;
        }
    }
    return 0;
}
