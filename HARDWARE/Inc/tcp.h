#ifndef _TCP_H
#define _TCP_H

#include "main.h"

#define SSID "xjy"
#define SSID_PASSWORD "112233qq"
#define TCP_IP "192.168.31.110"
#define TCP_PORT "2345"

extern uint8_t Flag_UART2_receive_OK;

void TCP_Init(void);
void Disable_Echo(void);
void Set_WorkMode(void);
void WIFI_Connect(void);
void TCP_Connect(void);
void Open_TTT(void);
void wait_OK(void);

#endif
