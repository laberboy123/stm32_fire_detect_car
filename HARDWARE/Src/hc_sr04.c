#include "hc_sr04.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

uint16_t count = 0; // 捕获值
float distance = 0; // 超声波测得的距离


/*************************
 *  @brief 超声波模块测距
 *  @param 无
 *  @return distance 距离 float类型
 *  @note 无
 *************************/
float Ultra_Distance(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    delay_us(50); // 根据说明书,需要提供至少10us的高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {};
    HAL_TIM_Base_Start(&htim3);
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET) {};
    HAL_TIM_Base_Stop(&htim3);

    count = __HAL_TIM_GET_COUNTER(&htim3);
    float distance = (count * 1.0 / 10 * 0.34) / 2;
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    delay_us(100);
    return distance;
}

