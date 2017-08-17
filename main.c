#define _XTAL_FREQ 4000000

// PIC16F876 Configuration Bit Settings
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = OFF         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <xc.h>

#include "lcd.h"
#include "esp8266.h"

//#define LED_DIR TRISB7
#define LED_PIN RB1
#define DHT11_PIN RA0
#define DHT11_TRIS TRISA0

void dht_init() {
    __delay_ms(1); // 1ms
    DHT11_TRIS = 0;
    DHT11_PIN = 1;
}

unsigned char dht_poll() {
    unsigned char i = 0;
    unsigned char result = 0;
 
    DHT11_TRIS = 1;
    for (i = 0; i < 8; i++) {
        while (DHT11_PIN == 0);
        __delay_us(30); //30us
        if (DHT11_PIN == 1) {
            result |= (1 << (7 - i));
        }
        while (DHT11_PIN == 1);
    }
    return result;
}


void read_dht(unsigned char *dht_dat) {
    unsigned char dht_in;
    unsigned char i;
    
    dht_init();
    
    DHT11_PIN = 0;  //port value is low
    __delay_ms(18); // 18ms
    DHT11_PIN = 1;  //port value is high
    __delay_us(30); // 20-40us
    DHT11_TRIS = 1; //make pin input

    dht_in = DHT11_PIN;
    if (dht_in) {
        //GlobalErr = 1;
        return;
    }
    
    __delay_us(80); // 80us
    dht_in = DHT11_PIN;
    if (!dht_in) {
        //GlobalErr = 2;
        return;
    }
    __delay_us(80); // 80us
    for (i = 0; i < 5; i++) {
        dht_dat[i] = dht_poll();
    }
    DHT11_PIN = 1;
    DHT11_TRIS = 0;
}

void printDataDHT11(unsigned char *dht_dat) {
        //LCD_Clear();
        LCD_Set_Cursor(1,0);

        char result[16];
        sprintf(result, "T: %d C, ", dht_dat[0]);
        LCD_putrs(result);
        
        sprintf(result, "H: %d %%", dht_dat[2]);
        LCD_putrs(result);
}

void main(void) {
    char data[32];
    uint16_t len = 0;
    unsigned char dht_dat[4];
    
    __delay_ms(1000);
    
    ADCON1 = 0b0110; // port A digital mode
    TRISB = 0x00; // All port B output 
    
    dht_init();
    read_dht(dht_dat);

     // Open led
    LED_PIN = 1;
    
    LCD lcd = { &PORTB, 2, 3, 4, 5, 6, 7 }; // PORT, RS, EN, D4, D5, D6, D7
    
    __delay_ms(1000);

    LCD_Init(lcd);

    LCD_Clear();
    LCD_Set_Cursor(0,0);
    LCD_putrs("I");

    Initialize_ESP8266(); 
    
    esp8266_isStarted();
    LCD_putrs("S");
     
    esp8266_mode(1);
    LCD_putrs("M");
    
    esp8266_mux(1);
    LCD_putrs("X");
    
    esp8266_connect("yerayblu","Somostres3");
    LCD_putrs("C");

    esp8266_server(1, "8080");
    LCD_Set_Cursor(0,7);
    LCD_putrs("SERVER");

    LCD_Set_Cursor(0,0);
    LCD_putrs(" IP: ");
    
    unsigned char myIp[];
    esp8266_ip2(myIp);
    LCD_putrs(myIp);

    printDataDHT11(dht_dat);
    while (1) {
        len = esp8266_receive(data, 16, false);
        
        LCD_Clear();
        LCD_Set_Cursor(0,0);

        LCD_putrs_len(data, len);
        
        read_dht(dht_dat);
        printDataDHT11(dht_dat);
    }

}
