/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringPi.h>
#include <stdint.h>

#define MAXTIMINGS	85
#define DHTPIN		7

int* read_dht11_dat()
{
    uint8_t laststate	= HIGH;
    uint8_t counter		= 0;
    uint8_t j = 0, i;
    static int dht11_dat[5] = { 0, 0, 0, 0, 0 };
    static int error[1] = { 0 };

    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode( DHTPIN, OUTPUT );
    digitalWrite( DHTPIN, LOW );
    delay(18);
    /* then pull it up for 40 microseconds */
    digitalWrite( DHTPIN, HIGH );
    delayMicroseconds(40);
    /* prepare to read the pin */
    pinMode( DHTPIN, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHTPIN ) == laststate )
        {
            counter++;
            delayMicroseconds(1);
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHTPIN );

        if ( counter == 255 )
            break;

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            dht11_dat[j / 8] <<= 1;
            if ( counter > 50 )	//Angepasst
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }

    if ( (j >= 40) &&
         (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
    {
        return dht11_dat;
    }else  {
        return error;
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int count = 5; //Anzahl der Werte pro Mittelwert

    int sockfd, newsockfd, portno;
    int * values;
    int temp[count];
    float temp_average = 0, hum_average = 0;
    int hum[count];


    if ( wiringPiSetup() == -1 ){
        exit( 1 );
    }

    for (int i=0;i<count;){
        values = read_dht11_dat();
        if (values[0]!=0){ //Luftfeuchtigkeit von 0 kommt real kaum vor
            printf( "Humidity = %d.%d %% Temperature = %d.%d *C\n", values[0], values[1], values[2], values[3] );
            temp[i] = values[2];
            hum[i] = values[0];
            i++;
        }
        delay( 1000 ); /* wait 1sec to refresh */
    }

    //Calculate Average
    for (int i=0;i<count;i++){
        temp_average = temp_average + temp[i];
        hum_average = hum_average + hum[i];

        printf("T: %f, H: %f\n",temp_average,hum_average);
    }

    temp_average = temp_average / count;
    hum_average = hum_average / count;

    char message[64];
    snprintf(message, sizeof(message), "temp: %f; hum: %f\n", temp_average, hum_average);

    socklen_t clilen;
    char buffer[256];

    struct sockaddr_in serv_addr, cli_addr;

    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0){
        error("ERROR opening socket");
        fprintf(stderr,"ERROR opening socket\n");
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }
    bzero(buffer,256);

    n = read(newsockfd,buffer,255);
    if (n < 0) {
        error("ERROR reading from socket");
    }

    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,message,64);

    if (n < 0){
        error("ERROR writing to socket");
    }


    close(newsockfd);
    close(sockfd);
    return 0;
}
