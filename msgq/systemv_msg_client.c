/*
 * Client program
 *           to demonstrate interprocess commnuication
 *           with System V message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'

struct message_text {
	int qid;
	char buf[200];
};

struct message {
	long message_type;
	struct message_text message_text;
};

int main(int argc, char **argv)
{
	key_t server_queue_key;
	int server_qid, myqid, ret = 0;
	struct message my_message, return_message;

	// create my client queue for receiving messages from server
	myqid = msgget(IPC_PRIVATE, 0660);
	if (myqid == -1) {
		perror("msgget: myqid");
		exit(1);
	}

	server_queue_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
	if (server_queue_key == -1) {
		perror("ftok");
		exit(1);
	}
	server_qid = msgget(server_queue_key, 0);
	if (server_qid == -1) {
		perror("msgget: server_qid");
		exit(1);
	}

	my_message.message_type = 1;
	my_message.message_text.qid = myqid;

	printf("Please type a message: ");

	while (fgets(my_message.message_text.buf, 198, stdin)) {
		// remove newline from string
		int length = strlen(my_message.message_text.buf);

		if (my_message.message_text.buf[length - 1] == '\n')
			my_message.message_text.buf[length - 1] = '\0';

		// send message to server
		ret = msgsnd(server_qid, &my_message,
				sizeof(struct message_text), 0);
		if (ret == -1) {
			perror("client: msgsnd");
			exit(1);
		}

		// read response from server
		ret = msgrcv(myqid, &return_message,
				sizeof(struct message_text), 0, 0);
		if (ret == -1) {
			perror("client: msgrcv");
			exit(1);
		}

		// process return message from server
		printf("Message received from server: %s\n\n",
				return_message.message_text.buf);

		printf("Please type a message: ");
	}
	// remove message queue
	ret = msgctl(myqid, IPC_RMID, NULL);
	if (ret == -1) {
		perror("client: msgctl");
		exit(1);
	}

	printf("Client: bye\n");

	exit(0);
}
