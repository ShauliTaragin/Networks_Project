
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
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
  exit(0);
}
