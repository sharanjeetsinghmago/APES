#include <stdio.h>
#include <pthread.h>
#include "temp_task.c"

pthread_t temp_id, light_id, socket_id;

void *func_temp()
{
	printf("Temperature Thread Started\n");
	if(temp_init() == -1)
		printf("Error Initializing Temperature Task\n");
  	else
		//get_temp();
		while(1)
		{
			read_temp_data_reg();
		}
	printf("Temperature Thread Finished\n");
}

void *func_light()
{
	printf("Light Thread Started\n");
	//while(1);
	printf("Light Thread Finished\n");
}

void *func_socket()
{
	printf("Socket Thread Started\n");
	printf("Socket Thread Finished\n");
}

int main()
{
	pthread_create(&temp_id, NULL, &func_temp, NULL);
	pthread_create(&light_id, NULL, &func_light, NULL);
	pthread_create(&socket_id, NULL, &func_socket, NULL);

	//get_temp();

	//pika_func();

	pthread_join(temp_id, NULL);
	pthread_join(light_id, NULL);
	pthread_join(socket_id, NULL);
}