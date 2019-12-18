#ifndef __SHM_SYSTEMV_COMMON_H
#define __SHM_SYSTEMV_COMMON_H

// Buffer data structures
#define MAX_BUFFERS 10
#define SHARED_MEMORY_KEY "./shared_memory_key"
#define SEM_MUTEX_KEY "./sem-mutex-key"
#define SEM_BUFFER_COUNT_KEY "./sem-buffer-count-key"
#define SEM_SPOOL_SIGNAL_KEY "./sem-spool-signal-key"
#define PROJECT_ID 'K'

struct shared_memory {
	char buf[MAX_BUFFERS][256];
	int buffer_index;
	int buffer_print_index;
};

#endif
