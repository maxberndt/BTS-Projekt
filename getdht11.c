#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error();

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    if (argc < 3) {
       printf("Usage: %s hostname port\n", argv[0]);
       error("Wrong usage\n");
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0){
        error("ERROR opening socket\n");
    }


    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("ERROR, no such host\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
      error("ERROR connecting");
    }

    printf("Press Enter to get values...\n");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
      error("ERROR writing to socket\n");
    }

    bzero(buffer,256);
    n = read(sockfd,buffer,255);

    if (n < 0){
      error("ERROR reading from socket\n");
    }

    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

void error(const char *msg)
{
    fprintf(stderr, msg);
    exit(1);
}
