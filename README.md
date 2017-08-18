# pic-wifi
In this project, I use pic 16f876 to control wifi module and write messages in lcd 16x2. Also, I add sensor dht11 to control temperature and humidity, this data are printed in display.

Components
==========
* Microcontroller: 16F876
* Wifi module: ESP8266 - ESP07
* LCD 16x2 compatible with hd44780
* Sensor DHT11

Diagram
=======

Program
=======
File main.c contains the principal main program, the file esp8266.c is a library to control wifi module and lcd.c is the library to manage lcd display.

I use PORTB to control lcd is important to configure LVP = OFF to ot work.
```
#pragma config LVP = OFF 
```
Also use RA0 pin to comunicate with DHT, is important to configure this port how digital I/O.
```
ADCON1 = 0b0110;
```
