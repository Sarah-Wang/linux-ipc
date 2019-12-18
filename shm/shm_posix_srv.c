/*
 *
 *       logger.c: Write strings in POSIX shared memory to file
 *                 (Server process)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "common.h"
#include "shm_posix_common.h"

int main(int argc, char **argv)
{
	struct shared_memory *shm_ptr = NULL;
	sem_t *mutex_sem, *buffer_count_sem, *spool_signal_sem;
	int fd_shm, fd_log;
	char mybuf[256];
	int ret = 0;

	// Open log file
	fd_log = open(LOGFILE, O_CREAT | O_WRONLY | O_APPEND | O_SYNC, 0666);
	if (fd_log == -1)
		error("fopen");

	//  mutual exclusion semaphore, mutex_sem with an initial value 0.
	mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT, 0660, 0);
	if (mutex_sem == SEM_FAILED)
		error("sem_open");

	// Get shared memory
	fd_shm = shm_open(SHARED_MEM_NAME, O_RDWR | O_CREAT, 0660);
	if (fd_shm == -1)
		error("shm_open");

	ret = ftruncate(fd_shm, sizeof(struct shared_memory));
	if (ret == -1)
		error("ftruncate");

	shm_ptr = mmap(NULL, sizeof(struct shared_memory),
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_shm, 0);
	if (shm_ptr == MAP_FAILED)
		error("mmap");
	// Initialize the shared memory
	shm_ptr->buffer_index = 0;
	shm_ptr->buffer_print_index = 0;

	/* counting semaphore, */
	/* indicating the number of available buffers. */
	/* Initial value = MAX_BUFFERS */
	buffer_count_sem = sem_open(SEM_BUFFER_COUNT_NAME,
			O_CREAT, 0660, MAX_BUFFERS);
	if (buffer_count_sem == SEM_FAILED)
		error("sem_open");

	/* counting semaphore, */
	/* indicating the number of strings to be printed. */
	/* Initial value = 0 */
	spool_signal_sem = sem_open(SEM_SPOOL_SIGNAL_NAME, O_CREAT, 0660, 0);
	if (spool_signal_sem == SEM_FAILED)
		error("sem_open");

	// Initialization complete; now we can set mutex semaphore as 1 to
	// indicate shared memory segment is available
	ret = sem_post(mutex_sem);
	if (ret == -1)
		error("sem_post: mutex_sem");

	while (1) {  // forever
		// Is there a string to print? P (spool_signal_sem);
		ret = sem_wait(spool_signal_sem);
		if (ret == -1)
			error("sem_wait: spool_signal_sem");

		strcpy(mybuf, shm_ptr->buf[shm_ptr->buffer_print_index]);

		// Since there is only one process (the logger) using the
		// buffer_print_index, mutex semaphore is not necessary
		(shm_ptr->buffer_print_index)++;
		if (shm_ptr->buffer_print_index == MAX_BUFFERS)
			shm_ptr->buffer_print_index = 0;

		//Contents of one buffer has been printed.
		// One more buffer is available for use by producers.
		// Release buffer: V (buffer_count_sem);
		ret = sem_post(buffer_count_sem);
		if (ret == -1)
			error("sem_post: buffer_count_sem");

		// write the string to file
		ret = write(fd_log, mybuf, strlen(mybuf));
		if (ret != strlen(mybuf))
			error("write: logfile");
	}
}
