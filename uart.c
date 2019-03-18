#include <xc.h>
#include <p18f8722.h>
#include <string.h>
#define _XTAL_FREQ 10000000


void initUart(void){
    
    /**INIT UART 1*/
    TXSTA1 = 0x0; //reset register
    RCSTA1 = 0x0; //reset register
    RCSTA1bits.CREN = 1; //enable receiver
    TXSTA1bits.BRGH = 1; //hight speed baud rate
    RCSTA1bits.ADDEN = 1; //enable address detection
    SPBRG1 = 10; //Baud Rate Generator Register Low Byte
    TXSTA1bits.TXEN = 1; //enable transmitter
    RCSTAbits.SPEN = 1; //Enables the serial port and configures the SDAx and SCLx pins as the serial port pins
    TRISCbits.TRISC6 = 0; //define C6 as output
    TRISCbits.TRISC7 = 1; //define C7 as input
    
    /*INIT UART 2*/
    TXSTA2 = 0x0; //reset register
    RCSTA2 = 0x0; //reset register
    RCSTA2bits.CREN = 1; //enable receiver
    TXSTA2bits.BRGH = 1; //hight speed baud rate
    RCSTA2bits.ADDEN = 1; //enable address detection
    SPBRG2 = 10; //Baud Rate Generator Register Low Byte
    TXSTA2bits.TXEN = 1; //enable transmitter
    RCSTA2bits.SPEN = 1; //Enables the serial port and configures the SDAx and SCLx pins as the serial port pins
    TRISGbits.TRISG1 = 0; //define C6 as output
    TRISGbits.TRISG2 = 1; //define G2 as input
    
    
    //baud rate = Fosc/[16*(SPBRG1 + 1 )] = 56,8kbauds = 57,6kbaud en reel
}


void sendStringUart2(const char* data){
    
    int i = 0;
    int taille_tab = strlen(data);
    
    for(i = 0; i<= taille_tab; i++){
            //envoit sur moniteur serie via arduino
            while(!TXSTA2bits.TRMT); //wait for transmittion register emptyF
            TXREG2 = data[i]; //send value to transmition register
    }
}

void cleanSerial(){
    for(int i = 0; i<=100; i++){
       sendStringUart2("\n"); 
    }
}
