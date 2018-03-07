#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define LOG(format, ...) printf("[PID:%d] ",getpid()); printf(format, ##__VA_ARGS__)

typedef struct
{
	char str[20];
	size_t len;
	int usr_ctr;
}msg_str;

char* msg1 = "Ping from Process 1";
//char* msg3 = "Pjygfvjgy";
char* msg2 = "Ping Back from Process 2";

int main()
{
	
	int p1[2],p2[2],i,pid;

	if (pipe(p1)==-1)
    {
        LOG("[ERROR] Pipe call error\n");
        return 1;
    }
    if (pipe(p2)==-1)
    {
        LOG("[ERROR] Pipe call error\n");
        return 1;
    }

	pid = fork();

	if(pid > 0)
	{
		char *message_cptr;
		msg_str message_send;
		message_cptr = (char*)&message_send;
		
		memcpy(message_send.str,msg1,strlen(msg1)+1);
		message_send.len = strlen(message_send.str);
		message_send.usr_ctr = 1;
		//strcpy(&(mssg1.str),"Process 1 message");
		close(p1[0]);
		write(p1[1], message_cptr, sizeof(message_send));
		close(p1[1]);
		
		wait(NULL);
		
		close(p2[1]);

		char inbuf[sizeof(msg_str)];
		msg_str *msgptr;

		read(p2[0], inbuf, sizeof(msg_str));
		
		msgptr = (msg_str*)inbuf;

		printf("Message Recieved\nMessage: %s\nMessageLen: %d\nUSRLED: %d\n",msgptr->str,msgptr->len,msgptr->usr_ctr);

		close(p2[0]);
		exit(0);
	}
	else if(pid == 0)
	{
		//close(p2[1]);

		char inbuf[sizeof(msg_str)];
		msg_str *msgptr;		

		read(p1[0], inbuf, sizeof(msg_str));
		
		msgptr = (msg_str*)inbuf;
		printf("Message Recieved\nMessage: %s\nMessageLen: %d\nUSRLED: %d\n",msgptr->str,msgptr->len,msgptr->usr_ctr);
 

		close(p1[0]);
		close(p2[0]);
		
		char *message_cptr;
		msg_str message_send;
		message_cptr = (char*)&message_send;

		memcpy(message_send.str,msg2,strlen(msg2)+1);
		message_send.len = strlen(message_send.str);
		message_send.usr_ctr = 0;

		write(p2[1], message_cptr, sizeof(message_send));
		close(p2[1]);
		exit(0);
	}

}