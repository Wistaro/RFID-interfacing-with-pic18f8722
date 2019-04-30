#ifndef RFID_H
#define	RFID_H

void initRfid(void);
char echoRfid(void);
void getId(void);
char isIdOk();
char calibration(void);
char readDataRfid(void);
void sendDataRfid(char data);

void select_protocole(void);
char get_NFC_tag(void);
void sendStringUart2v2(const char* data);

char* ascii2hex(char input);

#endif	/* RFID_H */