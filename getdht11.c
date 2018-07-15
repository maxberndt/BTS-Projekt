#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <argp.h>
int parameter(int argc, char* argv[]){
    char z;
    long portnumber=2000;
	char *ptr;
      for(int i=2;i<argc;i++)
    {
       if (argv[0][0] == '-' )
            {
                    switch (argv[i][1])  {
                default:
                            printf("The parameters you have set are not clear.\n");
                            printf("Type -p and the port number to define it. It is set on 2000 as default\n");
                            printf("The IP-Address is set by default, if a connection is not possible, you are able to redefine it\n");
                            break;

                case 'h'| 'o' | '-' :
							printf("You have got to define the IP-Address as the first parameter. Please use only IPv4\n");
                            printf("Type -p and the port to define it. It is set on 2000 as default\n");
                            break;
                case 'p':   printf("The port number is now redefined\n");
                            portnumber=strtol(argv[i+1],&ptr,5);
							break;
            }
        }

    }
    return portnumber;
    }

void error(const char *msg) //if failure
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int portnumber=2000;
    portnumber=parameter(argc,argv);
    int sock, portno, n;
    struct sockaddr_in serv_addr; //structur erstellen
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        fprintf(stderr,"ERROR opening socket\n");

   server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, host is not found\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portnumber);
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        fprintf(stderr, "ERROR connecting\n");
    printf("Press Enter to start");
    fgets(buffer,255,stdin);
    n = write(sock,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sock);
    return 0;
}
