/*A server to add numbers received in message. */

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"
#define MAX_NUMBERS 500

int main(int argc, char **argv)
{
	int fd, fd_client, bytes_read, i;
	char buf[4096] = {0};
	char *return_fifo = NULL;
	char *numbers[MAX_NUMBERS] = {0};

	if ((mkfifo(SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
		perror("mkfifo");
		exit(1);
	}

	fd = open(SERVER_FIFO, O_RDONLY);
	if (fd == -1)
		perror("open");


	while (1) {
		// get a message
		memset(buf, '\0', sizeof(buf));
		bytes_read = read(fd, buf, sizeof(buf));
		if (bytes_read == -1)
			perror("read");
		if (bytes_read == 0)
			continue;

		if (bytes_read > 0) {
			return_fifo = strtok(buf, ", \n");

			i = 0;
			numbers[i] = strtok(NULL, ", \n");

			while (numbers[i] != NULL && i < MAX_NUMBERS)
				numbers[++i] = strtok(NULL, ", \n");

			int total_numbers = i;
			double sum = 0;
			unsigned int error = 0;
			char *ptr = NULL;
			double f;

			for (i = 0; i < total_numbers; i++) {
				f = strtod(numbers[i], &ptr);

				if (*ptr) {
					error = 1;
					break;
				}
				sum += f;
			}

			/* Send the result */
			fd_client = open(return_fifo, O_WRONLY);
			if (fd_client == -1) {
				perror("open: client fifo");
				continue;
			}

			if (error)
				sprintf(buf, "Error in input.\n");
			else
				sprintf(buf, "Sum = %.8g\n", sum);

			if (write(fd_client, buf, strlen(buf)) != strlen(buf))
				perror("write");

			if (close(fd_client) == -1)
				perror("close");
		}

	}
}
