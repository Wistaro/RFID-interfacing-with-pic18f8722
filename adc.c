/*
 * File:   adc.c
 * Author: wromigui
 *
 * Created on 5 février 2019, 17:12
 */


#include <xc.h>
#include <p18f8722.h>
#define _XTAL_FREQ 10000000

void Adc_init(void) //used to get datas from the temperature sensor (not implemented yet)
{

    ADCON0 = 0; // init
    ADCON1 = 0; // init
    ADCON2 = 0; // init
    
    ADCON0bits.CHS = 0x0; //SELECT AN0 
    ADCON1bits.PCFG = 0b1110;// Set all half channel as digital, and half as analog
    ADCON2bits.ADCS = 0x0; // Frequence d'oscillation/2
    ADCON2bits.ACQT = 0x1; // Select 2 Tad
    ADCON0bits.ADON = 1; // start acquisition 

    __delay_ms(5); //wait
}

unsigned char conv(void)
{
    ADCON0bits.GODONE = 1;
    
    while(ADCON0bits.GODONE); //wait for the conversion complete 
        //return (int)((ADRESH<<8)+ADRESL); //return the whole result on 16bytes
        return ADRESH; //return the first 8 bits
    
    
}