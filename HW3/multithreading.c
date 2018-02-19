
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include <signal.h>


#define INP_TXT_FILE		"Valentinesday.txt"

sem_t g_sem_lfile_write;

pthread_t master_id, child_1_id, child_2_id;

FILE *fp_master = NULL;
FILE *fp_ct1 = NULL;
FILE *fp_ct2 = NULL;

typedef struct thread_info
{
    char log_file_name[50];
}thread_info; 

struct node_t 
{
	int ch;
	int count;
	struct node_t *next;
};

void logtofile(char *buffer, int len, FILE *fp)
{
	sem_wait(&g_sem_lfile_write);
	fwrite(buffer, sizeof(char), len, fp);
	sem_post(&g_sem_lfile_write);
}

void sig_handler_ct1(int sig)
{
	char buffer[1000];
	memset(buffer, '\0', sizeof(buffer));
	
	if (sig == SIGUSR1 || sig == SIGUSR2)
	{
		printf("Caught signal in child thread 1: %d\n", sig);

	
		time_t end_time = time(NULL); 
		char *time_str = ctime(&end_time);

		sprintf(buffer, "[Child 1] Thread exit at %s\n", time_str);
		logtofile(buffer, strlen(buffer), fp_ct1);

		if (fp_ct1)
			fclose(fp_ct1);
	
		pthread_cancel(child_1_id);
	}
}

void sig_handler_ct2(int sig)
{
	char buffer[1000];
	memset(buffer, '\0', sizeof(buffer));
	
	if (sig == SIGUSR1 || sig == SIGUSR2)
	{
		printf("Caught signal in child thread 2: %d\n", sig);
		
		
		time_t end_time = time(NULL); 
		char *time_str = ctime(&end_time);

		sprintf(buffer, "[Child 2] Thread exit at %s\n", time_str);
		logtofile(buffer, strlen(buffer), fp_ct2);

		if (fp_ct2)	
			fclose(fp_ct2);
	
		pthread_cancel(child_2_id);

		return;
	}
}

void *func_master(void *arg)
{
    thread_info t_info = *(thread_info *)arg;

	char buffer[1000];
	memset(buffer, '\0', sizeof(buffer));

	fp_master = fopen(t_info.log_file_name, "a");
	if (!fp_master)
	{
		printf("Log file open failed in master\n");
	}

	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Master] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	logtofile(buffer, strlen(buffer), fp_master);
	
	printf("\n\n PID : %d\n\n",getpid());

	signal(SIGUSR1, sig_handler_ct1);
	signal(SIGUSR2, sig_handler_ct2);

	
    pthread_join(child_1_id, NULL);
    pthread_join(child_2_id, NULL);
	
	time_t end_time = time(NULL);
	time_str = ctime(&end_time);

	sprintf(buffer, "[Master] Thread exit at %s\n", time_str);
	logtofile(buffer, strlen(buffer), fp_master);
    
	fclose(fp_master);
	
	pthread_exit(NULL);

}


void add_new_char(struct node_t **head, int c)
{
	struct node_t *current = (*head), *prev = NULL;

	if (head != NULL && current == NULL)
	{
		/* Linked list is empty */
		struct node_t *new_node = (struct node_t *)malloc(sizeof(struct node_t));
	
		new_node->ch = c;
		new_node->count = 1;
		new_node->next = NULL;

		(*head) = new_node;		
		return;	
	}

	while(current != NULL)
	{
		if ((char)current->ch == (char)c || (char)current->ch == (((char)c) - 'A' + 'a'))
		{	
			current->count++;
			break;
		}

		prev = current;

		current = current->next;
	}

	if (current == NULL)
	{
		
		struct node_t *new_node = (struct node_t *)malloc(sizeof(struct node_t));
	
		new_node->ch = c;
		new_node->count = 1;
		new_node->next = NULL;

		prev->next = new_node;
	}

	return;
}


void scan_file(struct node_t **head)
{
	FILE *file = fopen(INP_TXT_FILE, "r");
	if (!file)
	{
		printf("File %s open failed\n", INP_TXT_FILE);
	}

	int c;

	do
	{
		c = fgetc(file);
		add_new_char(head, c);
		if (feof(file))
			break;
	}while(1);
}

void print_result(struct node_t *head)
{
	struct node_t *current = head;

	printf("Characters with 3 occurences in the file : ");
	while(current != NULL)
	{
		if (current->count == 3)
		{
			printf("%c ", (char)current->ch);
		}
		current = current->next;
	}
	printf("\n");
}

void *func_thread1(void *arg)
{
    thread_info t_info = *(thread_info *)arg;

	char buffer[1000];
    memset(buffer, '\0', sizeof(buffer));
	
	fp_ct1 = fopen(t_info.log_file_name, "a");
	if (!fp_ct1)
	{
		printf("Log file open failed in child 1\n");
	}
	
	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 1] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	logtofile(buffer, strlen(buffer), fp_ct1);

	
		struct node_t *head = NULL;
		struct node_t **last_ref_ptr = &head;
		scan_file(last_ref_ptr);

		print_result(*last_ref_ptr);

}

void *func_thread2(void *arg)
{
    thread_info t_info = *(thread_info *)arg;

	char buffer[1000];
	memset(buffer, '\0', sizeof(buffer));
		
	fp_ct2 = fopen(t_info.log_file_name, "a");
	if (!fp_ct2)
	{
		printf("Log file open failed in child 2\n");
	}

	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 2] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	logtofile(buffer, strlen(buffer), fp_ct2);

	while(1)
	{
		
		FILE *fp = fopen("/proc/stat", "r");
		char stat_buffer[3000];
		memset(stat_buffer, '\0', sizeof(stat_buffer));

		char cpu_util_str[] = "\n\nCPU Utilization: \n";
		strcpy(stat_buffer, cpu_util_str);
		int count = 0;

		count += strlen(cpu_util_str);

		while (!feof(fp))
		{
			stat_buffer[count++] = fgetc(fp);
		}
		stat_buffer[count++] = '\n';
	
		logtofile(stat_buffer, count-1, fp_ct2);
	
		fclose(fp);
		
		usleep(100000);
	}
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Incorrect usage\n");
        printf("Usage: ./multiThreadprog <file_name>\n");
        exit(1);
    }

    char file_name[50];
    memset(file_name, '\0', sizeof(file_name));
    strncpy(file_name, argv[1], strlen(argv[1]));
    printf("Log File Name: %s\n", file_name);

    if (!fopen(file_name, "r"))
    {
        printf("Log file %s doesn't exist. Creating one.\n", file_name);
        FILE *fp = fopen(file_name, "w");
        if (fp)
        {
            printf("Log file %s successfully created\n", file_name);
            fclose(fp);
        }
        else
        {
            printf("%s:%s:%d File open failed\n", __FILE__, __func__, __LINE__);
            exit(1);
        }
    }
    thread_info t_info;
    memset(&t_info, '\0', sizeof(thread_info));
    strcpy(t_info.log_file_name, file_name);
    sem_init(&g_sem_lfile_write, 0, 1);
    pthread_create(&master_id, NULL, &func_master, (void *)&t_info);
    pthread_create(&child_1_id, NULL, &func_thread1, (void *)&t_info);
    pthread_create(&child_2_id, NULL, &func_thread2, (void *)&t_info);

    pthread_join(master_id, NULL);

    return 0;
}



