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

#define LINE_SIZE 600
#define SIM_LENGTH 10

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
    //initialize port default value to 80 (if we get a port from console this will change)
    int port=80;
    char hostname[100];
    char dummy_holdurl[LINE_SIZE];
    sscanf(urllink, "http://%99[^:/]:%99d/%99[^\n]", hostname, &port, dummy_holdurl);
    printf("ip = \"%s\"\n", hostname);
    printf("port = \"%d\"\n", port);



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

    char request_holder[LINE_SIZE];
    sprintf(request_holder, "GET %s HTTP/1.0\r\nHOST: %s\r\n\r\n" , urllink , hostname);
    u_long  size_to_send = strlen(request_holder);

    char recv_holder[LINE_SIZE];
    char * wo_indents;
    size_t bytes = 0;
    int sent = 0;
    if (write(sock, request_holder, size_to_send))
    {
        // Read the response
        while ((bytes = read(sock, recv_holder, LINE_SIZE)) > 0) 
        {
            recv_holder[bytes] = '\0';

            if(fputs(recv_holder, stdout) == EOF)
            {
                printf("fputs() error\n");
            }

            wo_indents = strstr(recv_holder, "\r\n\r\n");
            printf("%s" , wo_indents);
        }          
    }
    printf("Exiting now.\n");

    close(sock); // close socket
    exit(0);
}
