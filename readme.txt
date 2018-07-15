===Compile===
Compile Server:
gcc -Wall -o dht11srv dht11srv.c -lwiringPi -lpthread

Compile Client:
gcc -o getdht11 getdht11.c

Warning about not protected function is normal.

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
Start Client via ./getdht11 + SERVER IP-ADRESS
Default Port is 2000, you can define another port by using the parameter -p + portnumber
Possible Parameters:
 -p Change portnumber
 -h Show basic help

===Usage Bash-Script===
Possible Parameters:
 -start Start Server in Background
 -restart Restart Server in Background
 -stop Server stops
 -status Shows Status of the Server
