/*
 * File:   adc.c
 * Author: wromigui
 *
 * Created on 5 février 2019, 17:12
 */


#include <xc.h>
#include <p18f8722.h>
#define _XTAL_FREQ 10000000

void Adc_init(void)
{

    ADCON0 = 0; // init
    ADCON1 = 0; // init
    ADCON2 = 0; // init
    
    ADCON0bits.CHS = 0x0; //SELECT AN0 whre the potetiometer is
    ADCON1bits.PCFG = 0b1110;// Set all half channel as digital, and half as analog
    ADCON2bits.ADCS = 0x0; // Frequence d'oscillation/2
    ADCON2bits.ACQT = 0x1; // Select 2 Tad
    ADCON0bits.ADON = 1; // start acquisition 

    __delay_ms(5); //wait
}

unsigned char conv(void)
{
    ADCON0bits.GODONE = 1;
    
    while(ADCON0bits.GODONE); //attente de la conversion 
        //return (int)((ADRESH<<8)+ADRESL); //on retourne les résultat
        return ADRESH; //on retourne les résultat
    
    
}