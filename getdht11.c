#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <argp.h>


void error(const char *msg){
    fprintf(stderr, msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        error("First parameter must be servers IP-Address\n");
    }

    int portnumber = 2000;

    while ((++argv)[0])
    {
        if (argv[0][0] == '-' )
        {
            switch (argv[0][1])  {

                default:
                    printf("Unknown parameter -%c\n\n", argv[0][1]);
                    break;
                case 'h':
                    printf("Usage: ./getdht11 + IP-Address (+ -p Portnumber)\n\n");
                    break;
                case 'p':
                    if (argv[0+1]!=NULL){
                        printf("Port: %s\n\n", argv[0+1]);
                        portnumber = atoi(argv[0+1]);
                    } else {
                        printf("Port not valid! Using default port...\n");
                    }
                    break;
            }
        }

    }

    int sock, n;
    struct sockaddr_in serv_addr; //structur erstellen
    struct hostent *server;

    char buffer[256];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        error("ERROR opening socket\n");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("ERROR, host is not found\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portnumber);
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting\n");
    }

    printf("Press Enter to get values");
    fgets(buffer,255,stdin);
    n = write(sock,buffer,strlen(buffer));
    if (n < 0){
        error("ERROR writing to socket\n");
    }

    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0){
        error("ERROR reading from socket\n");
    }
    printf("%s\n",buffer);
    close(sock);
    return 0;
}
