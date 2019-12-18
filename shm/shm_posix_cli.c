/*
 *
 *       client.c: Write strings for printing in POSIX shared memory object
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "common.h"
#include "shm_posix_common.h"

int main(int argc, char **argv)
{
	struct shared_memory *shm_ptr = NULL;
	sem_t *mutex_sem, *buffer_count_sem, *spool_signal_sem;
	int fd_shm;
	char mybuf[256] = {0};
	char buf[200] = {0};
	char *cp = NULL;
	int length = 0;
	time_t now;
	int len = 0;
	int ret = 0;

	//  mutual exclusion semaphore, mutex_sem
	mutex_sem = sem_open(SEM_MUTEX_NAME, 0, 0, 0);
	if (mutex_sem == SEM_FAILED)
		error("sem_open");

	// Get shared memory
	fd_shm = shm_open(SHARED_MEM_NAME, O_RDWR, 0);
	if (fd_shm == -1)
		error("shm_open");

	shm_ptr = mmap(NULL, sizeof(struct shared_memory),
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_shm, 0);
	if (shm_ptr == MAP_FAILED)
		error("mmap");

	// counting semaphore,
	// indicating the number of available buffers.
	buffer_count_sem = sem_open(SEM_BUFFER_COUNT_NAME, 0, 0, 0);
	if (buffer_count_sem == SEM_FAILED)
		error("sem_open");

	// counting semaphore,
	// indicating the number of strings to be printed.
	// Initial value = 0
	spool_signal_sem = sem_open(SEM_SPOOL_SIGNAL_NAME, 0, 0, 0);
	if (spool_signal_sem == SEM_FAILED)
		error("sem_open");

	printf("Please type a message: ");

	while (fgets(buf, 198, stdin)) {
		// remove newline from string
		length = strlen(buf);
		if (buf[length - 1] == '\n')
		buf[length - 1] = '\0';

		// get a buffer: P (buffer_count_sem);
		if (sem_wait(buffer_count_sem) == -1)
			error("sem_wait: buffer_count_sem");

		// There might be multiple producers. We must ensure that
		// only one producer uses buffer_index at a time.
		// P (mutex_sem);
		if (sem_wait(mutex_sem) == -1)
			error("sem_wait: mutex_sem");

		// Critical section
		now = time(NULL);
		cp = ctime(&now);
		len = strlen(cp);
		if (*(cp + len - 1) == '\n')
			*(cp + len - 1) = '\0';
		sprintf(shm_ptr->buf[shm_ptr->buffer_index],
				"%d: %s %s\n", getpid(),
				cp, buf);
		(shm_ptr->buffer_index)++;
		if (shm_ptr->buffer_index == MAX_BUFFERS)
			shm_ptr->buffer_index = 0;

		// Release mutex sem: V (mutex_sem)
		if (sem_post(mutex_sem) == -1)
			error("sem_post: mutex_sem");

		// Tell spooler that there is a string to print:
		// V (spool_signal_sem);
		if (sem_post(spool_signal_sem) == -1)
			error("sem_post: (spool_signal_sem");

		printf("Please type a message: ");
	}

	ret = munmap(shm_ptr, sizeof(struct shared_memory));
	if (ret == -1)
		error("munmap");
	exit(0);
}

