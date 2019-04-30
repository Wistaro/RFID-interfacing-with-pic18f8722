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
#include "door.h"

#define _XTAL_FREQ 10000000
#define SIZE_ID 16


// configuration bits 
#pragma config OSC = HS        
#pragma config FCMEN = OFF      
#pragma config WDT = OFF    
#pragma config LVP = OFF   

//global variables initialization
char respEcho;
char respDetect;

char isEchoEnd;
char isIdCheckEnd;
char isCalibrationEnd;
char isSelectProtocolEnd;
char isTagDetected;
char isDoorOpen;
char isDoorClose;

extern char waitDoor;

int response_id;
char* id;
char* expected_id;
char puissance_calibration;

extern char powerLvl[8];

/*functions prototypes*/
void initIO();
void toggleLed(char idLed);
void waitMs2(int ms);

extern char startCountEcho;

/*programme principal*/
int main(void){   

    initIO(); //init GPIO
    Adc_init();//init ADC
    initUart(); //init UART 1 and 2
    initTimer(); //init timer handler
    initRfid(); //init RFID
    //LCDInit(); //Init the LCD
    cleanSerial(); //clean the serial monitor 
    
    respEcho = 0;
    
    /*State machine*/
    isEchoEnd = 0;
    isIdCheckEnd = 0;
    isCalibrationEnd = 0;
    isSelectProtocolEnd=0;
    isTagDetected = 0;
    isDoorOpen = 0;
    isDoorClose = 0;     
    
    expected_id = "NFC FS2JAST"; //the id of our tag detector module
            
    toggleLed(1); //init ok, first led on (debug only)
    
    sendStringUart2("\n\n-------------------------------------\n");
    sendStringUart2("TP Microprocesseur 2A \n (c) Pierson & Romiguieres\n");
    sendStringUart2("-------------------------------------\n\n");
    __delay_ms(70);
    
    while(1){ //infinite loop
        
        
        //Part.1: checking communication between RFID reader and PIC18f (test echo)
        if(!isEchoEnd) {  
            sendStringUart2("\nDemarrage en cours...\n");
            sendStringUart2("Envoie de la commande echo...\n");
            
            respEcho = echoRfid(); //We send the echo, and we wait for the response. 
            
            sendStringUart2(" > Echo termine!");
            
            PORTD=0x00; //led reset
            isEchoEnd = 1; //now, communication test is done
  
            toggleLed(1);
            toggleLed(2);
            
        }else{
            startCountEcho = 0;
        }
        
        //Part.2: obtaining the identity of the RFID tag reader
        if(!isIdCheckEnd){
            sendStringUart2("\nRecuperation de l'identite du RFID...");
            sendStringUart2("\nIdentite attendue:");
            sendStringUart2((const char*)expected_id);
            sendStringUart2(" \nIdentite lue: ");
            __delay_ms(70);
            __delay_ms(70);

            getId();
           
            
            if(isIdOk() == 1){ //if the tag reader id match with the expected id (stored in a constant)
                sendStringUart2("\n > Identite confirmee!\n");
                toggleLed(3);
                isIdCheckEnd = 1;
            }else{
                toggleLed(4);
                sendStringUart2("\n > Identite non confirmee!\n");
                isIdCheckEnd = 0;
                isEchoEnd = 0;

            }          
            
        }
            
       //Part.3: Power calibration of the RFID tag reader     
       if(!isCalibrationEnd){
           sendStringUart2("Calibration du RFID en cours....");
           puissance_calibration = calibration();
           sendStringUart2("\nCalibration terminee");

           isCalibrationEnd = 1;
       }         
        
        //Part.4: Selecting the communication protocol
        if (!isSelectProtocolEnd){
            sendStringUart2("\n Selection du protocole en cours....");
            select_protocole();
            sendStringUart2("\n Protocole ISO/IEC 14443 Type A - 106 Kbs selectionne");
            
            isSelectProtocolEnd = 1;
        }

        //Part.5: Tag detection + obtaining the identity of the RFID tag
        if(!isTagDetected){
            sendStringUart2("\nDetection du tag en cours...");
            __delay_ms(70); //fix bug with 2 uarts
            __delay_ms(70); //fix bug with 2 uarts
            respDetect = get_NFC_tag();
            
            if(respDetect == 1){ //the current tag is known and allowed to open the door
                sendStringUart2("\nTag detecte!");
                sendStringUart2("\n Ouverture de la porte en cours.");
                isTagDetected = 1;

            }else if(respDetect == 2){ //unknown tag, the door stay closed :(
                __delay_ms(70);
                sendStringUart2("\n\rMauvais tag!\n");
                __delay_ms(70);

            }else{ //Any tag is detected
                __delay_ms(70);
                sendStringUart2("\n\rAucun tag detecte!\n");
                __delay_ms(70);

            }     
        }
        
        //Part.6: Door handler
        if(isTagDetected == 1 && isDoorClose == 0){ //In the beginning of the program, the door is supposed to be close
            
            
            openDoor(); //we open the door

            sendStringUart2("\nPorte ouverte!");
            sendStringUart2("\nPour fermer la porte, appuyez sur le bouton RA5  \n");
            
            isDoorOpen = 1;
            
            if(PORTAbits.RA5 == 0){ 
                sendStringUart2("\n Fermeture de la porte");
                closeDoor(); //Hold the door!
                sendStringUart2("\nPorte fermee!");
                isDoorClose = 0;
                (isTagDetected = 0;
            }            
            
        }
     }
    
    
}

void initIO(){
    
   TRISD=0x00;
   TRISC=0x00;
   
   PORTD=0x00;
   PORTC=0x00;
   
   TRISF = 0x00;
   PORTF = 0x00;
   
   TRISAbits.RA5 = 1;
   
    
}

void toggleLed(char idLed){ //useful function to toggle a led
    
    PORTD^=(0x01<<(idLed-1)); 
    
}

