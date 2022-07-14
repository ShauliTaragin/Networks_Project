#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PORT 5060

void printsin(struct sockaddr_in *s, char *str1, char *str2) {
  printf("%s\n", str1);
  printf("%s: ", str2);
  char adress[INET_ADDRSTRLEN];
  inet_ntop(AF_INET , &(s->sin_addr) , adress,INET_ADDRSTRLEN);
  printf("ip= %s , port= %d\n" , adress , s->sin_port);
}


int main(int argc, char *argv[])
{
  int socket_fd, cc, fsize;
  struct sockaddr_in  s_in, from;
  struct { char head; u_long  body; char tail;} msg;

  socket_fd = socket (AF_INET, SOCK_DGRAM, 0); // initialize a receiving socket socket

  bzero((char *) &s_in, sizeof(s_in));  // set all the socket structures with null values


  s_in.sin_family = (short)AF_INET;
  s_in.sin_addr.s_addr = htonl(INADDR_ANY); 
  s_in.sin_port = htons(SERVER_PORT+1); 
  printsin( &s_in, "RECV_UDP", "Local socket is:"); 
  fflush(stdout);//immediately flush out(e.g write to console) the contents of stdout.

  bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in));//binding the socket to the adress,


  for(;;) {
    fsize = sizeof(from);
    //Receive a connection from gateway.
    cc = recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr *)&from,&fsize);
    // printsin( &from, "recv_udp: ", "Packet from:");
    printf("Got data ::%c%ld%c\n",msg.head,(long) ntohl(msg.body),msg.tail); 
    fflush(stdout);
  }
  
  return 0;
}

