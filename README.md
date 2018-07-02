# BTS Projekt

Sensor Setup:
* PIN 1 -> 3.3V
* PIN 2 -> GPIO4 (PIN 7)
* PIN 3 -> / Nicht verbunden
* PIN 4 -> Ground

#Compile

Compile Server:
gcc -Wall -o server server.c -lwiringPi -lpthread
