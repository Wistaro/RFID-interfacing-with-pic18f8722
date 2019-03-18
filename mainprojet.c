/*Loading headers files*/
#include <p18f8722.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "adc.h"
#include "rfid.h"
#include "uart.h"
#include "timer.h"
#include "lcd.h"
#include "utils.h"

#define _XTAL_FREQ 10000000
#define SIZE_ID 16


// configuration bits 
#pragma config OSC = HS        
#pragma config FCMEN = OFF      
#pragma config WDT = OFF    
#pragma config LVP = OFF   

//initialisation variables globales
char respEcho;

char isEchoEnd;
char isIdCheckEnd;
char isCalibrationEnd;
char isSelectProtocolEnd;
char isTagDetected;

int response_id;
char* id;
char* expected_id;
char puissance_calibration;

extern char powerLvl[8];

/*Prototypes des fonctions*/
void initIO();
void toggleLed(char idLed);
void waitMs2(int ms);

extern char startCountEcho;

/*programme principal*/
int main(void){         
    initIO();
    Adc_init();
    initUart();
    initTimer();
    initRfid();
    cleanSerial();
    
    respEcho = 0;
    /*State machine*/
    isEchoEnd = 0;
    isIdCheckEnd = 0;
    isCalibrationEnd = 0;
    isSelectProtocolEnd=0;
    isTagDetected = 0;
            
    expected_id = "NFC FS2JAST4";
            
    toggleLed(1); //init ok, first led on 
    
    sendStringUart2("\n\n-------------------------------------\n");
    sendStringUart2("TP Microprocesseur 2A \n (c) Pierson & Romiguieres\n");
    sendStringUart2("-------------------------------------\n\n");
    __delay_ms(70);
    
    while(1){ //boucle infinie
        
        
        //envoie de l'echo
        if(isEchoEnd == 0) {  //si l'écho n'a pas encore été fait   
            sendStringUart2("\nDemarrage en cours...\n");
            sendStringUart2("Envoie de la commande echo...\n");
            
            respEcho = echoRfid(); //envoie de l'echo. Tant que celui-ci n'est pas terminé on reste ici 
            
            sendStringUart2(" > Echo termine!");
            
            PORTD=0x00; //reset des leds
            isEchoEnd = 1; //on indique que l'echo a déjà été fait ou est en oours
  
            toggleLed(1);
            toggleLed(2);
            
        }else{
            startCountEcho = 0;
        }
        
        //envoie de l'identité
        if(!isIdCheckEnd){
            sendStringUart2("\nRecuperation de l'identite du RFID...");
            sendStringUart2("\nIdentite attendue:");
            sendStringUart2((const char*)expected_id);
            sendStringUart2(" \nIdentite lue: ");
            getId();
            isIdCheckEnd = 1;
           
            
            if(isIdOk() == 1){
                sendStringUart2("\n > Identite confirmee!\n");
                toggleLed(3);
            }else{
                toggleLed(4);
                sendStringUart2("\n > Identite non confirmee!\n");
                __delay_ms(70);
                __delay_ms(70);
                __delay_ms(70);
            }          
            
        }
            
       //calibration du rfid     
       if(!isCalibrationEnd){
           sendStringUart2("Calibration du RFID en cours....");
           puissance_calibration = calibration();
           sendStringUart2("\nCalibration terminee");

           isCalibrationEnd = 1;
       }         
        
        // Selection du protocole
        if (!isSelectProtocolEnd){
            sendStringUart2("\n Selection du protocole en cours....");
            select_protocole();
            sendStringUart2("\n Protocole ISO/IEC 14443 Type A - 106 Kbs selectionne");
            
            isSelectProtocolEnd = 1;
        }
        
        if(!isTagDetected){
            sendStringUart2("\nDetection du tag en cours...");
            get_NFC_tag();
            sendStringUart2("\n Tag detecte!");
            isTagDetected = 1;
        }
        
     }
    
    
}

void initIO(){
    
   TRISD=0x00;
   TRISC=0x00;
   
   PORTD=0x00;
   PORTC=0x00;     
    
}

void toggleLed(char idLed){
    
    PORTD^=(0x01<<(idLed-1));
    
}

