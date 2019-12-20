We can create a FIFO from the shell using the mknod command.

$ mknod myfifo p
$ ls -ls
total 0
0 prw-rw-r-- 1 user1 user1 0 May 30 15:08 myfifo
$ rm myfifo
$ ls -ls
total 0

There is a specialized mkfifo command which could have been used in place of the mknod command, above.

$ mkfifo mynewfifo
$ file mynewfifo
mynewfifo: fifo (named pipe)
