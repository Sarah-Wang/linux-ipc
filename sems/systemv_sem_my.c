#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <time.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

static int sem_id;
static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);
static int done;

void *set_num(void)
{
	sleep(100);
	done = 1;
	if (!semaphore_v()) {
		printf("semaphore_v failed\n");
		return (void *)(-1);
	}
}

int main(int argc, char *argv[])
{
	key_t tk = 0;
	pthread_t tid;
	int i = 0;
	time_t now;

	done = 0;

	tk = ftok("./", 0);
	if (tk < 0) {
		printf("get token key failed\n");
		return -1;
	}

	sem_id = semget(tk, 1, IPC_CREAT | 0666);
	if (sem_id < 0) {
		printf("get sem_id failed\n");
		return -2;
	}

	if (!set_semvalue()) {
		printf("failed to init semaphore\n");
		return -3;
	}

	pthread_create(&tid, NULL, (void *)set_num, NULL);

	time(&now);
	printf("line 66 time is %ld\n", now);

	if (!(semaphore_p())) {
		printf("semaphore_p failed\n");
		exit(-1);
	}

	time(&now);
	printf("line 74 time is %ld\n", now);

	printf("done is: %d\n", done);

	if (!semaphore_v())
		exit(-1);

	printf("done is: %d\n", done);
}


/* 用于初始化信号量，在使用信号量前必须这样做 */
static int set_semvalue(void)
{
	union semun sem_union;

	sem_union.val = 0;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;
	return 1;
}

/* 删除信号量 */
static void del_semvalue(void)
{
	union semun sem_union;

	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}


/* 对信号量做减1操作，即等待P（sv） */
static int semaphore_p(void)
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = -1;//P()
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_p failed\n");
		return 0;
	}

	return 1;
}


/* 释放操作，使信号量变为可用，即发送信号V（sv） */
static int semaphore_v(void)
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = 1; // V()
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_v failed\n");
		return 0;
	}

	return 1;
}
