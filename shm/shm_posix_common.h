#ifndef __SHM_POSIX_COMMON_H
#define __SHM_POSIX_COMMON_H

// Buffer data structures
#define MAX_BUFFERS 10

#define LOGFILE "./example.log"

#define SEM_MUTEX_NAME "/sem-mutex"
#define SEM_BUFFER_COUNT_NAME "/sem-buffer-count"
#define SEM_SPOOL_SIGNAL_NAME "/sem-spool-signal"
#define SHARED_MEM_NAME "/posix-shared-mem-example"

struct shared_memory {
	char buf[MAX_BUFFERS][256];
	int buffer_index;
	int buffer_print_index;
};

#endif
