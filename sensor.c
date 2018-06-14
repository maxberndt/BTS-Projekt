/*
 *  dht11.c:
 *	Simple test program to test the wiringPi functions
 *	DHT11 test
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
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

int main( void )
{

	int *values;

	if ( wiringPiSetup() == -1 )
		exit( 1 );

	while ( 1 )
	{
		values = read_dht11_dat();
		if (values[0]!=0){ //Luftfeuchtigkeit von 0 kommt real kaum vor
			printf( "Humidity = %d.%d %% Temperature = %d.%d *C\n", values[0], values[1], values[2], values[3] );
		}
		delay( 1000 ); /* wait 1sec to refresh */
	}

	return(0);
}
