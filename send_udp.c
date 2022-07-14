#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int socket_fd;
  struct sockaddr_in  dest;
  struct hostent *hostptr; // we will use this to store information about the host
  struct { char head; u_long body; char tail; } msgbuf; //a template for the message data

  socket_fd = socket (AF_INET, SOCK_DGRAM, 0);//open the socket
  bzero((char *) &dest, sizeof(dest)); // set all the socket structures with null values
  // it is imperative to use bzero in order to make memory room for socket structures
  hostptr = gethostbyname(argv[1]);// hold host name given as an argument from console
  dest.sin_family = (short) AF_INET;
  bcopy(hostptr->h_addr, (char *)&dest.sin_addr,hostptr->h_length);//copy h_length bytes 
  //from the area pointed to by hostptr to the area pointed to by dest.sin_addr
  dest.sin_port = htons((u_short)0x3333);//translating a short number from host byte order to network byte order

  msgbuf.head = '<';
  msgbuf.body = htonl(getpid());//network byte order , same as above but long instead of short
  // the conversions above are important as without converting to the network byte order binding wont succeed
  msgbuf.tail = '>';
  //Send a connection to the receiver.
  sendto(socket_fd,&msgbuf,sizeof(msgbuf),0,(struct sockaddr *)&dest,
                  sizeof(dest));

  return 0;
}
