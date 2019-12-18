/*
 * Server program
 *           to demonstrate interprocess commnuication
 *           with System V message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* The pathname must be an existing and accessible file */
#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660

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
	key_t msg_queue_key;
	int ret = 0;
	int qid;
	struct message message;

	msg_queue_key = ftok(SERVER_KEY_PATHNAME, PROJECT_ID);
	if (msg_queue_key == -1) {
		perror("ftok");
		exit(1);
	}
	qid = msgget(msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS);
	if (qid == -1) {
		perror("msgget");
		exit(1);
	}
	printf("Server: Hello, World!\n");

	while (1) {
		// read an incoming message
		ret = msgrcv(qid, &message, sizeof(struct message_text), 0, 0);
		if (ret == -1) {
			perror("msgrcv");
			exit(1);
		}

		printf("Server: message received.\n");

		// process message
		int length = strlen(message.message_text.buf);
		char buf[20];

		sprintf(buf, " %d", length);
		strcat(message.message_text.buf, buf);

		int client_qid = message.message_text.qid;

		message.message_text.qid = qid;

		// send reply message to client
		ret = msgsnd(client_qid, &message,
				sizeof(struct message_text), 0);
		if (ret == -1) {
			perror("msgget");
			exit(1);
		}

		printf("Server: response sent to client.\n");
	}
}
