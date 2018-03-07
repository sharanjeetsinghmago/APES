#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdlib.h>

#define SH_MEM_NAME	"my_sm"
#define SEM_NAME 	"my_semaphore"

typedef struct{
    char str[20];
    int len;
    int usr_ctr;
}msg_str;


int main()
{
	printf("Currently in process 1\n");
	
	int sm_fd = shm_open(SH_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if(sm_fd < 0)
	{
		perror("Cannot access shared memory");
		exit(1);
	}

	int sm_size = sizeof(msg_str);

	int ret = ftruncate(sm_fd, sm_size);
	if(ret < 0)
	{
		perror("Ftruncate error");
		exit(1);
	}

	msg_str * shared_mem = mmap(NULL, sm_size, PROT_READ | PROT_WRITE, MAP_SHARED , sm_fd, 0);
	if(shared_mem < 0)
	{
		printf("[ERROR] mmap error: %s\n",strerror(errno));
		return -1;
	}

	sem_t * sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
	if(sem == NULL)
	{
		printf("[ERROR] Sem open Failed:%s\n",strerror(errno));
		return -1;	
	}

	char msg[] = "Hello from Process1";
    	char * payload_cptr;
    	msg_str payloadSend = {0};
    	payload_cptr = (char*)&payloadSend;

    	strcpy(payloadSend.str,msg);
    	payloadSend.len = strlen(payloadSend.str);
    	payloadSend.usr_ctr = 1;

    	memcpy(shared_mem, &payloadSend, sm_size);

    	sem_post(sem);

    	msg_str payloadRecv = {0};
		payload_cptr = (char*)&payloadRecv;

    	memcpy(&payloadRecv, shared_mem, sm_size);

    	printf("[INFO] Message From Shared Mem\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadRecv.str,payloadRecv.len,payloadRecv.usr_ctr);

		ret = shm_unlink(SH_MEM_NAME);
	if(ret < 0)
	{
		printf("[ERROR] Cannot destroy Shared Mem: %s",strerror(errno));
		return -1;
	}

	return 0;
}
