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
#define PORT 9999

int main(int argc, char *argv[])
{
    // first lets get the url as a parameter,parse it and resolve it to an adress
    struct addrinfo *res; 
    char *hostaddr;
    struct sockaddr_in *saddr;
    // url variables
    char *urllink;
    // struct hostent *serv;


    if (argc != 2)
    { // check that we received strictly 1 hostname as an argument
        perror("Usage: hostnamelookup <hostname>\n");
        exit(1);
    }

    urllink = argv[1];

    int port;
    char hostname[100];
    char page[200];
    sscanf(urllink, "http://%99[^:]:%99d/%99[^\n]", hostname, &port, page);
    printf("ip = \"%s\"\n", hostname);
    printf("port = \"%d\"\n", port);
    printf("page = \"%s\"\n", page);



    //check hostname that was parsed from url is correct
    if (0 != getaddrinfo(hostname, NULL, NULL, &res))
    { 
        fprintf(stderr, "Error in resolving hostname %s\n", hostname);
        exit(1);
    }
    // serv = gethostbyname(hostname);
    saddr = (struct sockaddr_in *)res->ai_addr; // cast to the adress struct
    hostaddr = inet_ntoa(saddr->sin_addr);      // find host adress

    printf("Address for %s is %s\n", hostname, hostaddr);

    // now that we got the host adress lets start the regular procces of opening a socket and
    // creating a connection with the server

    int sock;
    struct sockaddr_in cli_name;
    int count;
    int value;

    printf("Client is alive and establishing socket connection.\n");

    sock = socket(AF_INET, SOCK_STREAM, 0); // initialize the socket to communicate with server
    if (sock < 0)
    {
        perror("Error opening channel");
        close(sock);
        exit(1);
    }

    bzero(&cli_name, sizeof(cli_name)); // set all the socket structure with null values
    cli_name.sin_family = AF_INET;
    cli_name.sin_addr.s_addr = inet_addr(hostaddr);
    cli_name.sin_port = htons(port); // network byte order for the port

    // memcpy(&cli_name.sin_addr.s_addr, serv->h_addr, serv->h_length);

    if (connect(sock, (struct sockaddr *)&cli_name, sizeof(cli_name)) < 0) // attempt to make a connection on a socket
    {
        perror("Error establishing communications");
        close(sock);
        exit(1);
    }
    int size_of_request =0;
    size_of_request += strlen("GET %s HTTP/1.0\r\n");
    size_of_request += strlen("HOST: %s\r\n");
    size_of_request += strlen("\r\n");
    char* request_holder = (char*)malloc(size_of_request);
    sprintf(request_holder, "GET %s HTTP/1.0\r\nHOST: %s\r\n" , page , hostname);
    strcat(request_holder,"\r\n");
    u_long  size_to_send = strlen(request_holder);

    ssize_t bytes = 0;
    int sent = 0;
    do {
        bytes = write(sock, request_holder + sent, size_to_send - sent);
        if (bytes < 0) {
            perror("Error writing message to socket");
        }
        if (bytes == 0) {
            printf("Writing all data is successful!\n");
            break;
        }
        sent += bytes;
    } while (sent < size_to_send);


    char buffer[4000];
    ssize_t bytes_r = 0;
    int recv = 0;
    memset(buffer , 0 , sizeof(buffer));
    int zero_buf = sizeof(buffer)-1;
    do {
        bytes_r = read(sock, buffer + recv, zero_buf - recv);
        fprintf(stdout,"%s\n",buffer);
        fflush(stdout);
        if (bytes_r < 0) {
            perror("Error reading message from socket");
        }
        if (bytes_r == 0) {
            printf("Writing all data is successful!\n");
            break;
        }
        recv += bytes_r;
    } while (recv < zero_buf);
    free(request_holder);
    printf("Exiting now.\n");

    close(sock); // close socket
    exit(0);
}
