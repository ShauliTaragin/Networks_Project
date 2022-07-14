#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>          
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>    
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

#define SIM_LENGTH 10 
// #define IP_ADDRESS "127.0.1.1" 
#define PORT 1337 

int main(int argc, char* argv[])
{ 
  //first lets get the hostname as a parameter and resolve it to an adress
  
  struct addrinfo* res;//this struct will hold the adrress info and give it to us via getadrr function 
  char* hostname;
  char* hostaddr;
  struct sockaddr_in* saddr;
  
  if (argc != 2) { //check that we received strictly 1 hostname as an argument
    perror("Usage: hostnamelookup <hostname>\n");
    exit(1);
  }

  hostname = argv[1];
  
  if (0 != getaddrinfo(hostname, NULL, NULL, &res)) {//check if hostname is correct
    fprintf(stderr, "Error in resolving hostname %s\n", hostname);
    exit(1);
  }
  
  saddr = (struct sockaddr_in*)res->ai_addr;//cast to the adress struct
  hostaddr = inet_ntoa(saddr->sin_addr);//find host adress

  printf("Address for %s is %s\n", hostname, hostaddr);


  //now that we got the host adress lets start the regular procces of opening a socket and 
  //creating a connection with the server

  int sock; 
  struct sockaddr_in cli_name; 
  int count;
  int value; 

  printf("Client is alive and establishing socket connection.\n");
  
  
  sock = socket(AF_INET, SOCK_STREAM, 0);//initialize the socket to communicate with server
  if (sock < 0)
    { perror ("Error opening channel");
      close(sock);
      exit(1);
    }
      

  bzero(&cli_name, sizeof(cli_name));// set all the socket structure with null values
  cli_name.sin_family = AF_INET; 
  cli_name.sin_addr.s_addr = inet_addr(hostaddr); 
  cli_name.sin_port = htons(PORT);//network byte order for the port 


  if (connect(sock, (struct sockaddr *)&cli_name, sizeof(cli_name)) < 0)//attempt to make a connection on a socket
    { perror ("Error establishing communications");
      close(sock);
      exit(1);
    }


  for (count = 1; count <= SIM_LENGTH; count++)
    { read(sock, &value, 4);//read what was received from socket
      printf("Client has received %d from socket.\n", value);
    }

  printf("Exiting now.\n");

  close(sock); //close socket
  exit(0); 

} 
