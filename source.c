#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#define SERVER_PORT 5060

int main(int argc, char *argv[])
{
    int socket_fd;
    struct sockaddr_in dest;
    struct hostent *hostptr; // we will use this to store information about the host
    struct
    {
        char head;
        u_long body;
        char tail;
    } msgbuf; // a template for the message data

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // open the socket
    bzero((char *)&dest, sizeof(dest));         // set all the socket structures with null values
    hostptr = gethostbyname(argv[1]); // hold host name given as an argument from console
    dest.sin_family = (short)AF_INET;
    bcopy(hostptr->h_addr, (char *)&dest.sin_addr, hostptr->h_length); 
    dest.sin_port = htons(SERVER_PORT); // translating a short number from host byte order to network byte order
    int num = 0;
    for (;;)
    {
        msgbuf.head = '<';
        msgbuf.body = htonl(num); // network byte order 
        msgbuf.tail = '>';
        // Send a connection to the receiver.
        sendto(socket_fd, &msgbuf, sizeof(msgbuf), 0, (struct sockaddr *)&dest,
               sizeof(dest));
        num+=1;
        sleep(1);
    }
    return 0;
}
