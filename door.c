/*
 * File:   rfid.c
 * Author: lpierson
 *
 * Created on 22 février 2019, 09:00
 */

#define t4 10
#define t0 50
#define t1 20
#define t3 10


#include <xc.h>
#include <stdio.h>
#include <string.h>



#define _XTAL_FREQ 10000000

void openDoor(void){ //Simulate a door opening by activating the stepper motor in one direction
    for(int i = 0; i<=30; i++){
    PORTFbits.RF6 = 1;
    __delay_ms(2);
    PORTFbits.RF6 = 0;
    __delay_ms(2);
     PORTFbits.RF6 = 1;
    __delay_ms(2);
    PORTFbits.RF6 = 0;
    __delay_ms(2);
     PORTFbits.RF6 = 1;
    __delay_ms(2);
    PORTFbits.RF6 = 0;
    __delay_ms(2);
    }
    
}

void closeDoor(void){ //Simulate a door closing by activating the stepper motor in the other direction

   
    for(int i = 0; i<=40; i++){
    PORTFbits.RF6 = 1;
    __delay_ms(1);
    PORTFbits.RF6 = 0;
    __delay_ms(1);
    PORTFbits.RF6 = 1;
    __delay_ms(1);
    PORTFbits.RF6 = 0;
    __delay_ms(1);
    PORTFbits.RF6 = 1;
    __delay_ms(1);
    PORTFbits.RF6 = 0;
    __delay_ms(1);
    }
    
    
}

