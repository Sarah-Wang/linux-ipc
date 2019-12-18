#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>

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

int main(int argc, char *argv[])
{
	char message = 'X';
	int i = 0;

	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

	if (argc > 1) {
		if (!set_semvalue()) {
			fprintf(stderr, "Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}

		message = argv[1][0];
		sleep(2);
	}

	for (i = 0; i < 10; ++i) {
		if (!semaphore_p())
			exit(EXIT_FAILURE);

		printf("%c", message);

		// 清理缓冲区，然后休眠随机时间
		fflush(stdout);
		sleep(rand() % 3);

		// 离开临界区前再一次向屏幕输出数据
		printf("%c", message);
		fflush(stdout);

		// 离开临界区，休眠随机时间后继续循环
		if (!semaphore_v())
			exit(EXIT_FAILURE);
		sleep(rand() % 2);
	}

	sleep(10);
	printf("\n%d - finished\n", getpid());

	/* 如果程序是第一次被调用，则在退出前删除信号量 */
	if (argc > 1) {
		sleep(3);
		del_semvalue();
	}
	exit(EXIT_SUCCESS);
}

static int set_semvalue(void)
{
	union semun sem_union;

	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;

	return 1;
}

static void del_semvalue(void)
{
	union semun sem_union;

	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

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
