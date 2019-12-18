Shared memory is the fastest IPC mechanism.
A program using POSIX shared memory calls needs to be linked with -lrt.

Refs to:
https://www.softprayog.in/programming/interprocess-communication-using-system-v-shared-memory-in-linux
https://www.softprayog.in/programming/interprocess-communication-using-posix-shared-memory-in-linux

commands:
 >./shared_memory_key
 >./sem-mutex-key
 >./sem-buffer-count-key
 >./sem-spool-signal-key
 ./systemv_spooler
 ./systemv_client
 ./systemv_client


For posix share memory samples,
the messages from the client will be written to the LOGFILE by logger.
