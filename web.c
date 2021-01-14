#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> //for communication
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

//Instead of putting a seperate file,in here the webpage is embedded in this c program
//Declaring a variable that will contain the web page
char webpage[] = 

//Following two lines are necessary for the processor to act based on what the server sends to the browser
//Before putting the content in the web page,we have to type these two lines
"HTTP/1.1 200 OK\r\n"  //return and new line
"Content-Type: text/html; charset=UTF-8\r\n\r\n"

//html code
"<!DOCTYPE html>\r\n"
"<html><head><title> My Web Page </title>\r\n"
"<style> body{background-color: black}\r\n"
"h1{text-align:center; font-size: 50px; color:red;}</style></head>\r\n"
"<body><center><h1>Hello! This is my Web Page</h1><br>\r\n"
"<img src=original.jpg><br>\r\n"
"</center></body></html>\r\n";

char sendfile();

//main function
int main(int argc, char *argv[])
{ 

	//Declaring variables
	struct sockaddr_in server_addr, client_addr; 
	socklen_t sin_len = sizeof(client_addr);
	int fd_server;  //This value will be the return value from the socket function
	int fd_client;
	char buff[2048]; //storing the returned or the content send by the browser to the web server
	int fd; //holding the file descriptor of the file that we open
	int on =1;
	
	
	//calling the functions
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_server < 0)  //if something went wrong
	{
		perror("socket");
		exit(1);
	}
	
	//if there is no wrong, setup the socket properties
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)); 
	
	//initialize a server address structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);  //will have any permission when using 8080 as the port
	
	if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)  //-1 -> if something went wrong
	{
		perror("bind");
		close(fd_server);
		exit(1);
	}
	
	if(listen(fd_server, 10) == -1)  //10 -> number of connections simultaneously
	{
		perror("listen");
		close(fd_server);
		exit(1);
	}
	
	//while loop -> this is where the action starts
	while(1)
	{
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);  //waiting for the connection and accepts
		
		if(fd_client == -1)  //if something went wrong
		{
			perror("Connection failed....\n");
			continue;
		}
		else
		{
			printf("Got client connection.....\n");
		}
		
		if(!fork())
		{
			/* child process */
			close(fd_server);
			memset(buff, 0, 2048);  //create the buffer
			read(fd_client, buff, 2047);
			
			
			printf("%s\n", buff); //print the content in the buff(see what the client/browser send) 
			
			//sending the icon to the webpage
			if(!strncmp(buff, "GET /favicon.ico", 16))
			{
				fd = open("favicon.ico", O_RDONLY);
				sendfile(fd_client, fd, NULL, 190000);
				close(fd);
			}
			
			//sending the image to the webpage
			else if(!strncmp(buff, "GET /original.jpg", 16))
			{
				fd = open("original.jpg", O_RDONLY);
				sendfile(fd_client, fd, NULL, 22000);
				close(fd);
			}

			else
				//sending the web page
				write(fd_client, webpage, sizeof(webpage) -1);
			
			//closing the client
			close (fd_client);
			printf("------------------------------Closing------------------------------\n"); 
			exit(0);
		}	
			
				
			/* parent process */
			close(fd_client);
	}	
	

	return 0;
}
