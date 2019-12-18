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
static int done;

void *set_num(void)
{
	sleep(100);
	done = 1;
	if (sem_post(&done_sem) == -1) {
		printf("sem_post failed\n");
		return (void *)(-1);
	}
}

int main(int argc, char *argv[])
{
	pthread_t tid;
	int i = 0;
	time_t now;

	done = 0;

	if (sem_init(&done_sem, 0, 0) == -1) {
		perror("sem_init");
		exit(1);
	}

	pthread_create(&tid, NULL, (void *)set_num, NULL);

	time(&now);
	printf("line %d time is %ld\n", __LINE__, now);
	printf("done is: %d\n", done);

	if (sem_wait(&done_sem) == -1) {
		perror("sem_wait");
		exit(1);
	}

	time(&now);
	printf("line %d time is %ld\n", __LINE__, now);

	printf("done is: %d\n", done);

	if (sem_destroy(&done_sem) == -1) {
		perror("sem_destroy");
		exit(1);
	}

	exit(0);
}

