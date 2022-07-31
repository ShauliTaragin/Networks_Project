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


void printsin(struct sockaddr_in *s, char *str1, char *str2) {
  printf("%s\n", str1);
  printf("%s: ", str2);
  printf("ip= %s, port= %d", inet_ntoa(s->sin_addr), s->sin_port);
}

int main(int argc, char *argv[])
{
  int socket_fd, cc, fsize;
  struct sockaddr_in  s_in, from;
  struct { char head; u_long  body; char tail;} msg;

  socket_fd = socket (AF_INET, SOCK_DGRAM, 0); // initialize a socket

  bzero((char *) &s_in, sizeof(s_in));  // set all the socket structures with null values
  // it is imperative to use bzero in order to make memory room for socket structures

  s_in.sin_family = (short)AF_INET;
  s_in.sin_addr.s_addr = htonl(INADDR_ANY); //translating a long integer from host byte order to network byte order
  s_in.sin_port = htons((u_short)0x3333); //network byte order , same as above but short instead of long
  // the conversions above are important as without converting to the network byte order binding wont succeed
  printsin( &s_in, "RECV_UDP", "Local socket is:"); 
  fflush(stdout);//immediately flush out(e.g write to console) the contents of stdout.

  bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in));//binding the socket to the adress,
  //i.e. bind the socket to a unique local name

  for(;;) {
    fsize = sizeof(from);
    //Receive a connection from the sender.
    cc = recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr *)&from,&fsize);
    printsin( &from, "recv_udp: ", "Packet from:");
    //ntohl translates a long integer from network byte order to host byte order
    printf("Got data ::%c%ld%c\n",msg.head,(long) ntohl(msg.body),msg.tail); 
    fflush(stdout);
  }
  
  return 0;
}

