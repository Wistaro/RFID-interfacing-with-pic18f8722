/* 
 * File:   uart.h
 * Author: wromigui
 *
 * Created on 15 février 2019, 14:35
 */

#ifndef UART_H
#define	UART_H

void initUart(void);
void writeUart1(char val);
char readUart1(void);
void sendStringUart2(const char* data);
void cleanSerial();




#endif	/* UART_H */

