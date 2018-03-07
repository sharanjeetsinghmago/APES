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
#include <sys/wait.h>

#define SH_MEM_NAME 	"my_sm"
#define SEM_NAME 	"my_semaphore"

typedef struct
{
	char str[20];	
	int len;
	int usr_ctr;
}msg_str;


int main()
{
	printf("[INFO] Starting the process 2\n");	
	int shmem_fd = shm_open(SH_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if(shmem_fd < 0 )
	{
		printf("[ERROR] Cannot open Shared Mem: %s",strerror(errno));
		return -1;
	}

	int sm_size = sizeof(msg_str);
	void *shared_mem = mmap(NULL, sm_size, PROT_READ | PROT_WRITE, MAP_SHARED , shmem_fd, 0);
	if(shared_mem == (void*)-1)
	{
		printf("[ERROR] mmap error: %s\n", strerror(errno));
		return -1;
	}

	/* Creating a semaphore to sync the reads and writes between 2 processes */
	sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
	if(SEM_FAILED == sem)
	{
		printf("[ERROR] Sem open Failed:%s\n",strerror(errno));
		return -1;	
	}

	/* Waiting for the process 1 to post the sem after writing  to the shared mem */
	sem_wait(sem);
	//wait(NULL);

	msg_str payloadRecv = {0};
    char *payload_cptr = (char*)&payloadRecv;

    memcpy(payload_cptr,(char*)shared_mem, sm_size);

    printf("[INFO] Message From Proc 1 through Shared Mem\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadRecv.str,payloadRecv.len,payloadRecv.usr_ctr);

    const char* msg = "Hello from Process2";
    msg_str payloadSend = {0};
    payload_cptr = (char*)&payloadSend;

    memmove(payloadSend.str,msg,strlen(msg));
    payloadSend.len = strlen(payloadSend.str);
    payloadSend.usr_ctr = 1;

    /* Copy the contents of the payload into the share memory */
    memcpy((char*)shared_mem, payload_cptr, sm_size);

    /*Indicating the process 1 that the  has been written for Process 1 eyes only */
    sem_post(sem);

	/*Closing the shared memory handle*/
	int ret = close(shmem_fd);
	if(ret < 0)
	{
		printf("[ERROR] Cannot close Shared Mem: %s",strerror(errno));
		return -1;
	}
  sem_unlink(SEM_NAME);
	
	return 0;
}
