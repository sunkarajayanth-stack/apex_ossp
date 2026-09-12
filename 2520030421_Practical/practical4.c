#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_NAME "server_fifo"
#define BUFFER_SIZE 1024

int main()
{
    int fd;
    char buffer[BUFFER_SIZE];

    // Create FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    printf("Server started...\n");
    printf("Waiting for client messages...\n");

    while (1)
    {
        // Open FIFO for reading
        fd = open(FIFO_NAME, O_RDONLY);

        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Read message
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';

            printf("\nClient Message: %s\n", buffer);

            // Process message
            printf("Server: Processing request...\n");

            sleep(1);

            printf("Server Response: Message received successfully.\n");
        }

        close(fd);
    }

    unlink(FIFO_NAME);

    return 0;
}
