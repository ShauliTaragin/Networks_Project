#include <sys/types.h>
#include <unistd.h>          
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>     
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define SIM_LENGTH 10
#define PORT 1337

int main(void)
{ 
  int sock; 
  int connect_sock; 
  struct sockaddr_in serv_name; 
  size_t len; 
  int count;
  
  sock = socket(AF_INET, SOCK_STREAM, 0); //open socket   
    
  bzero(&serv_name, sizeof(serv_name)); // set all the socket structure with null values
  serv_name.sin_family = AF_INET;                                   
  serv_name.sin_port = htons(PORT);//network byte order for the port 

  bind(sock, (struct sockaddr *)&serv_name, sizeof(serv_name));//bind the socket to server
          
  listen(sock, 1);//listen for incoming requests 

  len = sizeof(serv_name);
  
  connect_sock = accept(sock, (struct sockaddr *)&serv_name, (socklen_t*)&len);//receive and accept the incoming requests

  for (count = 1; count <= SIM_LENGTH; count++)
    { write(connect_sock, &count, 4);//write data out of the buffer
      printf("Server has written %d to socket.\n", count);
    }

  close(connect_sock);//close the receiving socket
  close(sock);

}
