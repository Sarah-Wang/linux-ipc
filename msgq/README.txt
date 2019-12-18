Reference:
https://www.softprayog.in/programming/interprocess-communication-using-system-v-message-queues-in-linux
https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux

How to build:
mkdir build && cd build && cmake .. && make


How to run:
> /tmp/mqueue_server_key
(or touch /tmp/mqueue_server_key)
./systemv_msg_server &
./systemv_msg_client


Note:
1. The pathname of ftok() must be an existing and accessible file.
2.The systemv_msg_server process works with an infinite loop.
	It can be terminated with the kill -9 pid command.
	In real life, you would make it a service with scripts to start and stop it.
	If you kill the server, the queue is left behind, which can be removed with the ipcrm -q qid command.
	You can find the qid using the ipcs -q command.

