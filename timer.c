/*
 * File:   timer.c
 * Author: wromigui
 *
 * Created on 5 février 2019, 15:45
 */

#include <pic18f8722.h>
#define MAX_REACH_TIME 20

int cpt;
int toto;
extern char isEchoOk;
extern char startCountEcho;
char isEcho2;

void initTimer(){
    
    T0CONbits.TMR0ON = 1; //init timer
    T0CONbits.T08BIT =1; //timer 8 bits
    T0CONbits.T0CS = 0 ; //Fosc/4
    T0CONbits.PSA=0; // active diviseur de frequnce
    T0CONbits.T0PS=0xF; // Diviseur de frequence par 256
    
    INTCONbits.GIE=1; // Active les interruptions global
    INTCONbits.PEIE=1; 
    INTCONbits.TMR0IE=1; // Active les interruptions sur le timer 0  
    
    cpt = 0;

}

void interrupt routine_IT_timer0(){
    
    
    if(INTCONbits.TMR0IF && INTCONbits.TMR0IE){
        
        
        if (startCountEcho == 1 ){
        
            if (cpt >= MAX_REACH_TIME){
                cpt=0;
                isEcho2=0;    
                
            }else{
                cpt++;
                isEcho2 = 1;
                PORTD^=0x01<<7;
                
            }
        }
        
        INTCONbits.TMR0IF = 0;
    }
    
}
