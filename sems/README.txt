man sem_overview on Ubuntu.

A named semaphore is identified by a name of the form /somename;
 that is, a null-terminated string of up to NAME_MAX-4 (i.e., 251)
 characters consisting of an initial slash, followed by one or more
 characters, none of which are slashes.
 Two processes can operate on the same named semaphore by passing the same name to sem_open(3).

 On  Linux,  named  semaphores are created in a virtual filesystem,
 normally mounted under /dev/shm, with names of the form sem.somename.
 (This is the reason that semaphore names are limited to NAME_MAX-4
rather than NAME_MAX characters.)

If run ./posix_sem_named, the semaphores will be found in /dev/shm:
$ ls /dev/shm
sem.sem-buffer-count  sem.sem-mutex  sem.sem-spool-signal
