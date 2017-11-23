#include<stdio.h>
#include<winsock2.h>
#include <pthread.h>

void *connection_handler(void *);

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in server , client;
    int c;
    char *message,server_reply[2000];

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2222 );

    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
    }

    puts("Bind done");

    //Listen to incoming connections
    listen(s , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    while( 1 )
    {
    	new_socket = accept(s , (struct sockaddr *)&client, &c);
	    if (new_socket == INVALID_SOCKET)
	    {
	        printf("accept failed with error code : %d" , WSAGetLastError());
	    }
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &new_socket) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (new_socket < 0)
    {
        perror("accept failed");
        return 1;
	}

    closesocket(s);
    WSACleanup();

    return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message,client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    send(sock , message , strlen(message) , 0);
    
    message = "Now type something and i shall repeat what you type \n";
    send(sock , message , strlen(message) , 0);
    
    //Receive a reply from the server
    if((read_size = recv(sock , client_message , 2000 , 0)) == SOCKET_ERROR)
    {
        puts("recv failed");
    }

    puts("Reply received\n");

    //Add a NULL terminating character to make it a proper string before printing
    client_message[read_size] = '\0';
    puts(client_message);
         
    return 0;
} 

