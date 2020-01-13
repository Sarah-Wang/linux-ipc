#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <time.h>


static sem_t done_sem;

void *put_sem(void)
{
	while (1) {
		sleep(5);
		if (sem_post(&done_sem) == -1) {
			printf("sem_post failed\n");
			return (void *)(-1);
		}
	}
}

void *get_sem(void)
{
	while (1) {
		printf("before sem_wait\n");
		if (sem_wait(&done_sem) == -1) {
			perror("sem_wait");
			exit(1);
		}
		usleep(1000);
		printf("get sem and do sth...\n");
	}
}

int main(int argc, char *argv[])
{
	pthread_t tid;

	if (sem_init(&done_sem, 0, 0) == -1) {
		perror("sem_init");
		exit(1);
	}

	/* pthread_create(&tid, NULL, (void *)put_sem, NULL); */
	pthread_create(&tid, NULL, (void *)get_sem, NULL);

	while (1) {
		sleep(5);
		if (sem_post(&done_sem) == -1)
			printf("sem_post failed\n");
		printf("\n\nafter sem_post\n\n");
	}


	if (sem_destroy(&done_sem) == -1) {
		perror("sem_destroy");
		exit(1);
	}

	exit(0);
}

