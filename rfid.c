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
#include "rfid.h"



#define _XTAL_FREQ 10000000

char startCountEcho;
char isEchoOk;
extern char* id;
extern char* expected_id;
extern char isEcho2;

char powerLvl[8] =  {0x00, 0xFC, 0x7C, 0x3C, 0x5C, 0x6C, 0x74, 0x70};

void initRfid(){
    
    PORTCbits.RC6 = 0;
    __delay_ms(t4);
    PORTCbits.RC6 = 1;
    __delay_ms(t0);
    PORTCbits.RC6 = 0;
    __delay_ms(t1);
    PORTCbits.RC6 = 1;
    __delay_ms(t3);
    
    
}
char echoRfid(void){
    
    char data = 0x55;
    char isEchoOk = 1; 
    startCountEcho = 0;    
    
     while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
        TXREG1 = data; //send value to transmition registe
        
        //wait for reception 
        while(!PIR1bits.RC1IF){ 
            
            
            startCountEcho=1; //start counting 
            //if counting is done, resend statup sequence
            
            if(isEcho2 == 0){
                
                 while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
                 
                 TXREG1 = data; //send value to transmition register
                 
                
                isEchoOk = 1;
                isEcho2 = 1;
                startCountEcho = 0;
                PORTD^=0x01<<6;
            }
            
      
        }
     
        return 1;
    
    
}

void getId(void){
    
    char data_getId_1 = 0x01;
    char data_getId_2 = 0x00;
    char response_id;
    
    while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
        TXREG1 = data_getId_1; //send value to transmition registe
        
    while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
        TXREG1 = data_getId_2; //send value to transmition registe
        
        //__delay_ms(5.5);
       
       for(int i = 0; i<=15; i++){
           
           while(!PIR1bits.RC1IF); //wait for reception
           response_id = RCREG1;
           
           id[i] = RCREG1;
       }
    
}

char isIdOk(){
    
    char validId = 1;
    
    for(int i = 3; i<=15; i++){
        
       if(id[i] != expected_id[i-3]){
           validId = 0;
        }else{
            while(!TXSTA2bits.TRMT); //wait for transmittion register emptyF
            TXREG2 = id[i]; //send value to transmition register
        }
        
    }
    
    return validId;
    
}

void sendDataRfid(char data){
        
     while(!TXSTA1bits.TRMT);
      TXREG1 = data;
}

char readDataRfid(void){
      
    char buff_output;
    
     while(!PIR1bits.RC1IF);
      buff_output = RCREG1;
      
      return buff_output;
}

char calibration(void){
    
    char stringInput[13] = {0x07, 0x0E, 0x03, 0xA1, 0x00, 0xF8, 0x01, 0x18, 0x00, 0x20, 0x60, 0x60, 0x00};
    
    int i, j;
    int k = 0;
    char power_calib;
    char output_read;
    char response[3] =  {0x57, 0x57, 0x57};
    
    output_read = readDataRfid(); //clean to avoid fake datas
    output_read = readDataRfid(); 

    for(k = 0; k<=8; k++){
        
        for(i = 0; i<=12; i++){      
            sendDataRfid(stringInput[i]);              
        }

         sendDataRfid(powerLvl[k]);

         sendDataRfid(0x3F);
         sendDataRfid(0x01);

         //output_read = readDataRfid(); //clean to avoid fake datas
         //output_read = readDataRfid(); 
         
         output_read = readDataRfid();
          response[0] = output_read+0x30;
          
         if(output_read == 0x82){ //en cas d'erreur!
             output_read = readDataRfid();
             k = 0; //on réinitialise la boucle
             sendStringUart2v2("\nErreur lors de la calibration. On retente. \n");
         }else{
              for(j = 2; j<=3; j++){
                output_read = readDataRfid();
                 response[j-1] = output_read+0x30;
                }
         }

        

         if(response[2] == '2'){

             if(k >= 1){ 
                 sendStringUart2v2("OK-");
                 power_calib = powerLvl[k];
                 
                 k = 8;
                 
             }

         }else{
             sendStringUart2v2("NO-");
         }
         
         sendStringUart2v2(k+0x30);
         
         
   }
    
    return power_calib;
}

void select_protocole(void){
    sendDataRfid(0x02); 
    sendDataRfid(0x02); //length
    sendDataRfid(0x02); //type ISO/IEC 14443 TypeA
    sendDataRfid(0x00); // Parameters 106 Kbs
    
    if (readDataRfid()==0x00){
        sendStringUart2v2("\nProtocole selectionne");
    }
    else {
         sendStringUart2v2("\nErreur lors de la selection du protocole ");
    }
    readDataRfid();
}
void get_NFC_tag(void){
    char buff_output;
    char respCmd1[6];
    char* respCmd2;
    char command1[4] = {0x04, 0x02, 0x26, 0x07}; //REQA
    char command2[5] = {0x04, 0x03, 0x93, 0x20, 0x08};
    
    
    for(int i = 0; i<5; i++){
        sendDataRfid(command2[i]);
    }

    for(int j = 0; j<8; j++){
        buff_output = readDataRfid();
        sendStringUart2v2((const char*)ascii2hex(buff_output));     
    }
    //in buff_output we have the lenght of the datas
    for(int k = 0; k<buff_output; k++){
        buff_output = readDataRfid();
        sendStringUart2v2((const char*)ascii2hex(buff_output));     
    }
    
    
    sendStringUart2v2("\nEND debug\n");
}

void sendStringUart2v2(const char* data){
    
    int i = 0;
    int taille_tab = strlen(data);
    
    for(i = 0; i<= taille_tab; i++){
            //envoit sur moniteur serie via arduino
            while(!TXSTA2bits.TRMT); //wait for transmittion register emptyF
            TXREG2 = data[i]; //send value to transmition register
    }
}

char* ascii2hex(char input){
    
    char* output;
    
    switch(input) {

   case 0x80:
      output = "0x80 "; 
      break; 
   case 0x05:
      output = "0x05 "; 
      break; 
   case 0x44:
      output = "0x44 "; 
      break; 
   case 0x00:
      output = "0x00"; 
      break;
    case 0x28:
      output = "0x28 "; 
      break;    
    case 0x08:
      output = "0x08 ";   
      break;    
    case 0x88:
      output = "0x88 "; 
      break;    
    case 0x04:
      output = "0x04 "; 
      break;
    case 0xA8:
      output = "0xA8 "; 
      break;   
    case 0xD5:
      output = "0xD5 "; 
      break;   
    case 0xF1:
      output = "0xF1 "; 
      break;   
   default: 
       output = "?";
}
    return output;
}


