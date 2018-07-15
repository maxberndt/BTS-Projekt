===Compile===

Compile Server:
gcc -Wall -o dht11srv dht11srv.c -lwiringPi -lpthread

Compile Client:
gcc -o getdht11 getdht11.c


===Setup Pi===
Sensor ->  Pi
PIN 1  ->  3.3V
PIN 2  ->  GPIO4 (PIN 7)
PIN 4  ->  Ground


===Config File===
To change the startup configuration, change the included dht11srv.conf file.
Format: Number of Measurements, Port, Delay (ms) between Measurements
If you want to use another filename, use parameter -c + Filename

===Usage Server===
Start Server via ./dht11srv
Possible Parameters:
 -v (Verbose)
 -c Change startup config file name
 -h Show basic help

The server will perform measurements and wait for a client to send the data.

===Usage Client===
Start Client via ./dht11srv IP-ADRESS PORT
