#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef struct{
	char string[50];
	int string_length;
	int USR_Led_Control;
}message;

int main(int argc, char* argv[])
{
	int socket_server,accept_var;
    char buff[256];
    struct sockaddr_in server_addr, client_addr;
	int portno = 3000;
	message *ptr;
    message mesg_struct;
	
	socket_server = socket(AF_INET,SOCK_STREAM,0);
	if(!(socket_server))
		printf("ERROR opening socket\n");
	else printf("Successfully created client socket\n");

	server_addr.sin_family = AF_INET;
	struct hostent *host = gethostbyname(argv[1]);
	memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    	server_addr.sin_port = htons(portno);
	
	if (connect(socket_server,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) 
        printf("ERROR connecting\n");
	
	
	//write
    ptr = &mesg_struct;
    strcpy(mesg_struct.string, "client to server");
       mesg_struct.string_length = strlen(mesg_struct.string);
       mesg_struct.USR_Led_Control = 0;
	int send_var = send(socket_server, (void*)(&mesg_struct), sizeof(mesg_struct),0);
	if (send_var < 0) 
         printf("ERROR sending to socket\n");

	
    int read_var = read(socket_server,buff,sizeof(message));
    if (read_var < 0) 
         printf("ERROR reading from socket\n");
	 ptr = (message*)(buff);
    printf("Client read string: %s, String length = %d, USR led status: %d\n", ptr->string, ptr->string_length, ptr->USR_Led_Control);
    
    close(socket_server);
    return 0;
	
	
}
