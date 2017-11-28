#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>


int create_socket(char *host, int port) 
{
	int socket_desc, client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[100];
	int j = 0;
	char server_msg[100] = {"\0"};
	char welcome_msg[100] = {"Testserver Dominik\n\0"};
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int portd;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        fprintf(stderr,"Could not create socket\n");
    }
    fprintf(stdout, "Socket created\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;//IPv4 AF_INET=IPv6
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        fprintf(stderr,"bind failed. Error: %d\n", errno);
        return 1;
    }
    fprintf(stdout,"bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    fprintf(stdout,"Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        fprintf(stderr,"accept failed\n");
        return 1;
    }
    fprintf(stdout,"Connection accepted\n");
    //Send welcome Message to client
    if(send(client_sock , welcome_msg , strlen(welcome_msg) , 0) == -1)
			fprintf(stderr,"Send failed %s\n");
    //Get Source IP and Source Port
    len = sizeof addr;
	getpeername(client_sock, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    portd = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    printf("Host: %s, Port: %d\n", ipstr, portd);
    //Receive a message from client limited on 100 characters
    while( (read_size = recv(client_sock , client_message , 100 , 0)) > 0 )
    {
		fprintf(stdout, "Message Org: %s Length:%d\n", client_message, read_size);
		strncpy(server_msg,client_message,read_size);
		//Send back the client message
		if(send(client_sock , server_msg , 100 , 0) == -1)
			fprintf(stderr,"Send failed %s\n", server_msg);
			
		for(int i = 0; i <= 100; i++)
		{
			client_message[i] = '\0';
			server_msg[i] = '\0';
		}
		//break;
		
    }
    return 0;
}
void INThandler()
{
  printf("Close Connection\n");
  exit(0);
}
int main(int argc, char **argv)
{	
	int sockets;
	signal(SIGINT, INThandler);
	sockets = create_socket("localhost", 60134);//choose Port 60134 for Server
	
	return 0;
}

