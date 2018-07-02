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
#include <pthread.h>

#define MAXTIMINGS	85
#define DHTPIN		7

int* read_dht11_dat();
void error();

int count = 5;
int portnr = 2000; //Default Port 2000
int delay_t = 1000;
int verbose = 0;

float temp_average = 0, hum_average = 0, temp_current = 0, hum_current = 0;
int * values;
char message[64];
int sockfd, newsockfd, portno;

static void *measure_thread(void* val) {

    int temp[count];
    int hum[count];

    snprintf(message, sizeof(message), "Not enough values. Please wait!");

    while (1){

        for (int i=0;i<count;){
            values = read_dht11_dat();
            if (values[0]!=0){ //Luftfeuchtigkeit von 0 kommt real kaum vor
                if (verbose){
                    printf( "Humidity = %d.%d %% Temperature = %d.%d *C\n", values[0], values[1], values[2], values[3] );
                }
                temp[i] = values[2];
                hum[i] = values[0];
                i++;
            }
            delay( delay_t );
        }

        //Calculate Average
        for (int i=0;i<count;i++){
            temp_average = temp_average + temp[i];
            hum_average = hum_average + hum[i];
        }

        temp_current = temp_average / count;
        hum_current = hum_average / count;


        //Reset Values
        temp_average = 0;
        hum_average = 0;

        if (verbose){
            printf("Average T: %f, Average H: %f\n",temp_current,hum_current);
        }
        snprintf(message, sizeof(message), "Temperatur: %f; Humidity: %f\n", temp_current, hum_current);

    }

    return NULL;
}

static void *socketconnect(void* val){



    socklen_t clilen;
    char buffer[256];

    struct sockaddr_in serv_addr, cli_addr;

    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0){
        error("ERROR opening socket\n");
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = portnr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding. Port already in use?\n");
    }

    while (1) {

        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept\n");
        }
        bzero(buffer, 256);

        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            error("ERROR reading from socket\n");
        }

        printf("Here is the message: %s\n", buffer);
        n = write(newsockfd, message, 64);

        if (n < 0) {
            error("ERROR writing to socket\n");
        }
    }

    return NULL;

}


void error(const char *msg)
{
    fprintf(stderr, msg);
    exit(1);
}

static void *quit(void* val) {

    while (1){
        if (getchar() == 'q')
            printf("Server is terminating...But he will be back...\n");
            close(newsockfd);
            close(sockfd);
            exit(1);
    }

    return NULL;

}

int main(int argc, char *argv[])
{


    while ((++argv)[0])
    {
        if (argv[0][0] == '-' )
        {
            switch (argv[0][1])  {

                default:
                    printf("Unknown parameter -%c\n\n", argv[0][1]);
                    break;
                case 'h':
                    printf("Help: BLABLABLA\n\n");
                    break;
                case 'v':
                    verbose = 1;
                    break;
            }
        }

    }

    if(verbose){
        printf("Verbose mode is active!\n\n");
        printf("Scanning dht11srv.conf File...\n");
    }

    FILE *config;

    config = fopen("dht11srv.conf", "r");

    if(NULL == config) {
        printf("Config File Error!\n"
               "Using Default Values. \n"
               "Port: 2000, delay: 1000 ms, 5 values per average");
    }

    else{

        fscanf(config,"%i,%i,%i\n",&count,&portnr,&delay_t);

        if (delay_t<1000){
            printf("Delay Time too short, must be over 1000ms. Setting it to 1000ms...\n");
            delay_t = 1000;
        }

        if (count == 5 && portnr == 2000 && delay_t == 1000){
            printf("Nah! Nah! Nah! Seems like the config file was not in the right format.\n"
                   "Using default Values...\n\n");
        }

        if (verbose){
            printf("Count: %i - Port: %i - Delay: %i ms\n\n",count,portnr,delay_t);
        }

    }

    pthread_t thread1,thread2,thread3;
    int rc;

    printf("========== SERVER UP ==========\n"
           "Terminate Server with q + Enter\n"
           "===============================\n\n");

    if ( wiringPiSetup() == -1 ){
        error("WiringPI  Error\n");
    }

    rc = pthread_create( &thread1, NULL, &measure_thread, NULL );

    if( rc != 0 ) {
        error("Error while creating Thread 1\n");
    }

    rc = pthread_create( &thread2, NULL, &socketconnect, NULL );

    if( rc != 0 ) {
        error("Error while creating Thread 2\n");
    }

    rc = pthread_create( &thread3, NULL, &quit, NULL );

    if( rc != 0 ) {
        error("Error while creating Thread 3\n");
    }

    pthread_join( thread1, NULL );
    pthread_join( thread2, NULL );
    pthread_join( thread3, NULL );

    return 0;
}



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
