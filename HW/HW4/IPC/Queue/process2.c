#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define SH_Q_NAME 		"/myIPCSharedQ"
#define SH_Q_QSIZE		8

#define MUTEX_NAME		"/mySharedRWMutex"
#define CONDVAR_NAME	"/mySharedRWCondVar"

typedef struct{
    char str[20];
    size_t len;
    uint8_t usr_ctr;
}msg_str;

int main()
{
	struct mq_attr mysharedQ_attr;
	mysharedQ_attr.mq_maxmsg = SH_Q_QSIZE;
	mysharedQ_attr.mq_msgsize = sizeof(msg_str);

	mqd_t mySharedQ = mq_open(SH_Q_NAME, O_CREAT | O_RDWR, 0666, &mysharedQ_attr);

	if(mySharedQ == (mqd_t)-1)
	{
		printf("[ERROR] QUEUE OPEN ERROR.: %s\n",strerror(errno));
		return -1;
	}

    char *payload_cptr;
    msg_str payloadRecv = {0};
    payload_cptr = (char*)&payloadRecv;

	int ret = mq_receive(mySharedQ, payload_cptr, sizeof(payloadRecv),0);
	if(ret == -1)
	{
		printf("[ERROR] Q Send error: %s\n",strerror(errno));
		return -1;
	}
	printf("[INFO] Message recd size: %d\n",ret);
	//payloadptr = (msg_str*)readbuf;
    printf("[INFO] Message Dequeued\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadRecv.str,payloadRecv.len,payloadRecv.usr_ctr);


    const char* msg = "Hello from Process2";
    msg_str payloadSend = {0};
    payload_cptr = (char*)&payloadSend;

    memmove(payloadSend.str,msg,strlen(msg));
    payloadSend.len = strlen(payloadSend.str);
    payloadSend.usr_ctr = 1;

	ret = mq_send(mySharedQ, payload_cptr, sizeof(payloadSend),0);
	if(ret == -1)
	{
		printf("[ERROR] Q Send error: %s\n",strerror(errno));
		return -1;
	}

	printf("[INFO] Message Queued\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadSend.str,payloadSend.len,payloadSend.usr_ctr);


	/*Closing the Q. Process 1 will destroy the queue */
	mq_close(mySharedQ);

	printf("[ERROR] QUEUE CLOSED\n");
	return 0;
}