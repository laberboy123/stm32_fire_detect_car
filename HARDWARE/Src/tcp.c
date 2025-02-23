#include "tcp.h"
#include "usart.h"

uint8_t Flag_UART2_receive_OK = 0;

void TCP_Init(void)
{
    Disable_Echo();
    wait_OK();

    Set_WorkMode();
    wait_OK();

    WIFI_Connect();
    wait_OK();

    TCP_Connect();
    wait_OK();

    Open_TTT();
}

void Disable_Echo(void)
{
    uint8_t cmd[10] = {"ATE0\r\n"};
    printf("disable echo......\r\n");
    HAL_UART_Transmit_DMA(&huart2, cmd, strlen((char *)cmd));
    HAL_Delay(1000);
}

void Set_WorkMode(void)
{
    printf("set workmode....\r\n");
    uint8_t cmd[] = "AT+WMODE=1,0\r\n";
    printf("cmd :%s\r\n", cmd);
    HAL_UART_Transmit_DMA(&huart2, cmd, strlen((char *)cmd));
    HAL_Delay(1000);
}

void WIFI_Connect(void)
{
    printf("wifi connect....\r\n");
    uint8_t cmd[100];
    sprintf((char *)cmd, "AT+WJAP=\"%s\",\"%s\"\r\n", SSID, SSID_PASSWORD);
    printf("cmd: %s\n", cmd);
    HAL_UART_Transmit_DMA(&huart2, cmd, strlen((char *)cmd));
    HAL_Delay(6000);
}

void TCP_Connect(void)
{
    printf("tcp connect....\r\n");
    uint8_t cmd[256];
    sprintf((char *)cmd, "AT+SOCKET=4,\"%s\",\"%s\"\r\n", TCP_IP, TCP_PORT);
    printf("cmd: %s\n", cmd);
    HAL_UART_Transmit_DMA(&huart2, cmd, strlen((char *)cmd));
    HAL_Delay(1000);
}

void Open_TTT(void)
{
    printf("open TTT....\r\n");
    uint8_t cmd[32];
    sprintf((char *)cmd, "AT+SOCKETTT\r\n");
    HAL_UART_Transmit_DMA(&huart2, cmd, strlen((char *)cmd));
    HAL_Delay(2000);

//    uint8_t tcp_hello[32];
//    sprintf((char *)tcp_hello, "HELLOWORLD\r\n");
//    HAL_UART_Transmit_DMA(&huart2, tcp_hello, strlen((char *)tcp_hello));
}

void wait_OK(void)
{
    uint32_t start = HAL_GetTick();
    uint32_t timeout = 5000; // Setting the timeout period (5 seconds)

    while ((HAL_GetTick() - start) < timeout) {
        if (Flag_UART2_receive_OK) {
            Flag_UART2_receive_OK = 0; // Clear Flags
            printf("Received OK\r\n");
            return;
        }
    }
    printf("Timeout waiting for OK\r\n");
}
