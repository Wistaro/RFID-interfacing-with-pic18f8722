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

void initRfid(){ //initialize RFID  - Send rfid init sequence
    
    PORTCbits.RC6 = 0;
    __delay_ms(t4);
    PORTCbits.RC6 = 1;
    __delay_ms(t0);
    PORTCbits.RC6 = 0;
    __delay_ms(t1);
    PORTCbits.RC6 = 1;
    __delay_ms(t3);
    
    
}
char echoRfid(void){ //send 0x55 and read the echo response
    
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
     
        //desactivate timer to avoid issues
        INTCONbits.TMR0IE=0;
        INTCONbits.GIE=0;
        
        return 1;
    
    
}

void getId(void){
    
    char data_getId_1 = 0x01; //firt command to send to get the id
    char data_getId_2 = 0x00; //second command to send to get the id
    char response_id;
    
    while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
        TXREG1 = data_getId_1; //send value to transmition registe
        
    while(!TXSTA1bits.TRMT); //wait for transmittion register emptyF
        TXREG1 = data_getId_2; //send value to transmition registe
        
       
       for(int i = 0; i<=15; i++){
           
           while(!PIR1bits.RC1IF); //wait for reception
           response_id = RCREG1;
           
           id[i] = RCREG1;
       }
    
}

char isIdOk(){ //verify is the id read is the correct id expected
    
    char validId = 1;
    
    for(int i = 3; i<=13; i++){
        
       if(id[i] != expected_id[i-3]){
           validId = 0;
        }else{
            while(!TXSTA2bits.TRMT); //wait for transmittion register emptyF
            TXREG2 = id[i]; //send value to transmition register
        }
        
    }
    
    return validId;
    
}

void sendDataRfid(char data){ //send byte to uart1 - RFID 
        
     while(!TXSTA1bits.TRMT);
      TXREG1 = data;
}

char readDataRfid(void){ //read byte from uart1 - RFID
      
    char buff_output;
    
     while(!PIR1bits.RC1IF);
      buff_output = RCREG1;
      
      return buff_output;
}

char calibration(void){ //send powers to rfid and optimize the power
    
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
char get_NFC_tag(void){
    
    __delay_ms(70);
    __delay_ms(70);
    __delay_ms(70);
    __delay_ms(70);
    __delay_ms(70);
    
    char buff_output;
    char expected_uid[5] = {0x27, 0x58, 0xDA, 0xC5, 0x60};
    char uid[5];
    char isGoodTag = 0;
    char tagOk = 1;
    char respCmd1[7];
    char respCmd2[10];
    char command1[4] = {0x04, 0x02, 0x26, 0x07}; //REQA
    char command2[5] = {0x04, 0x03, 0x93, 0x20, 0x08};
    
    //sendStringUart2v2("\nSend commande 1\n");
    for(int i = 0; i<4; i++){
        sendDataRfid(command1[i]);
    }
    
    buff_output = readDataRfid();
    respCmd1[0] = buff_output;
    
    //sendStringUart2v2("\nRead commande 1\n");

    if(respCmd1[0] == 0x87){ //no tag detected
        buff_output = readDataRfid();   
        return 0;
    }

    buff_output = readDataRfid(); 
    buff_output = readDataRfid(); 
    buff_output = readDataRfid(); 
    
    buff_output = readDataRfid(); 
    buff_output = readDataRfid(); 
    
    buff_output = readDataRfid(); 

    for(int w = 0; w<5; w++){
        sendDataRfid(command2[w]);
    }

    respCmd2[0] = readDataRfid();
    respCmd2[1] = readDataRfid();
    respCmd2[2] = readDataRfid();
    respCmd2[3] = readDataRfid(); 
    respCmd2[4] = readDataRfid();
    respCmd2[5] = readDataRfid();
    respCmd2[6] = readDataRfid();
    respCmd2[7] = readDataRfid();
    respCmd2[8] = readDataRfid();
    respCmd2[9] = readDataRfid();
    
    
    for(int i = 2; i<7; i++){
        if(respCmd2[i] != expected_uid[i-2]){
            tagOk = 0;
        }
    }
    
    if(tagOk == 1){      
        return 1;
    }else{
        return 2;
    }
    
   
}

void sendStringUart2v2(const char* data){ //send datas to serial monitor through an Arduino
    
    int i = 0;
    int taille_tab = strlen(data);
    
    for(i = 0; i<= taille_tab; i++){
            while(!TXSTA2bits.TRMT); //wait for transmittion register empty
            TXREG2 = data[i]; //send value to transmition register
    }
}

char* ascii2hex(char input){ 
  /* This is the fastest way we found to converter a byte in hex format to a string with this hex format
  * We assume that a better method exist, but we did'nt have the time to improve this function!
  */ 
    char* output;
    char buff[2];
    
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
    case 0x33:
      output = "0x33 ";
      break;
    case 0x16:
      output = "0x16 "; 
      break;  
    case 0x26:
      output = "0x26 "; 
      break;
    case 0x94:
      output = "0x94 "; 
      break;    
    case 0xA3:
      output = "0xA3 "; 
      break;   
    case 0x41:
      output = "0x41 "; 
      break;  
    case 0xAA:
      output = "0xAA "; 
      break;
    case 0xC5:
      output = "0xC5 "; 
      break;    
    case 0x8D:
      output = "0x8D "; 
      break; 
    case 0x87:
      output = "0x87 "; 
      break; 
    case 0x86:
      output = "0x86 "; 
      break; 
    case 0x27:
      output = "0x27 "; 
      break;
    case 0x58:
      output = "0x58 "; 
      break;
    case 0xDA:
      output = "0xDA "; 
      break;
    case 0x60:
      output = "0x60 "; 
      break;  
   default: 
       output = "? ";
       break;
}
    return output;
}


