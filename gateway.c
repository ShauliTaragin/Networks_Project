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
#include <time.h>
#include <stdlib.h>


void printsin(struct sockaddr_in *s, char *str1, char *str2)
{
    printf("%s\n", str1);
    printf("%s: ", str2);
    char adress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(s->sin_addr), adress, INET_ADDRSTRLEN);
    printf("ip= %s , port= %d\n", adress, s->sin_port);
}

int main(int argc, char *argv[])
{
    int socket_fd, cc, fsize;
    struct sockaddr_in s_in, from;
    struct
    {
        char head;
        u_long body;
        char tail;
    } msg;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // initialize a receiving socket

    bzero((char *)&s_in, sizeof(s_in)); // set all the socket structures with null values

    //get port number p from console
    char * portnumptr = argv[2];
    int port = strtol(portnumptr, NULL, 10);

    s_in.sin_family = (short)AF_INET;
    s_in.sin_addr.s_addr = htonl(INADDR_ANY); // translating a long integer from host byte order to network byte order
    s_in.sin_port = htons(port);       // network byte order 
    printsin(&s_in, "RECV_UDP", "Local socket is:");
    fflush(stdout); 
    bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in)); // binding the socket to the adress,

    // sender part of gateway
    int socket_sender;
    struct sockaddr_in dest;
    struct hostent *hostptr; 
    // struct
    // {
    //     char head;
    //     u_long body;
    //     char tail;
    // } msgbuf; 

    socket_sender = socket(AF_INET, SOCK_DGRAM, 0); // open the sending socket
    bzero((char *)&dest, sizeof(dest));        

    hostptr = gethostbyname(argv[1]); // hold host name given as an argument from console
    dest.sin_family = (short)AF_INET;
    bcopy(hostptr->h_addr, (char *)&dest.sin_addr, hostptr->h_length); 
    dest.sin_port = htons(port + 1); // sending on port number +1 then port we received

    srand(time(0));
    for (;;)
    {
        // first receive from source
        fsize = sizeof(from);
        // Receive a connection from the sender.
        cc = recvfrom(socket_fd, &msg, sizeof(msg), 0, (struct sockaddr *)&from, &fsize);
        // printsin(&from, "recv_udp: ", "Packet from:");

        printf("Got data ::%c%ld%c\n", msg.head, (long)ntohl(msg.body), msg.tail);
        fflush(stdout);

        // generate random number
        double rand_num = ((float)rand()) / ((float)RAND_MAX);
        if (rand_num > 0.5)
        {
            // send to sink the msg received from source
            sendto(socket_sender, &msg, sizeof(msg), 0, (struct sockaddr *)&dest,
                   sizeof(dest));
            sleep(1);
        }
    }
    return 1;
}
