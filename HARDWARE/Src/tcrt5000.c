#include "tcrt5000.h"
#include "adc.h"
#include "usart.h"

#define ADC_BUF_SIZE 3

uint16_t adc_values[ADC_BUF_SIZE]; // 用于存储 PA6、PA7、内部参考电压
float voltage[2] = {0.0};

/*************************
 *  @brief 启动 ADC 电压转换
 *  @param 无
 *  @return 无
 *  @note 无
 *************************/
void Start_ADC_Conversion(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);    // 校准ADC
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, ADC_BUF_SIZE); // 启动ADC
}


/*************************
 *  @brief 输出 ADC 检测的电压值
 *  @param 无
 *  @return 无
 *  @note 无
 *************************/
void ADC_Output(void)
{
    float Vref = 1.2 * (4095.0  / (float)adc_values[2]);
    for(int i = 0; i<2; i++) {
        voltage[i] = (adc_values[i] / 4095.0) * Vref;
    }
    printf("left: %.2f, right: %.2f, inside: %.2f\n", voltage[0], voltage[1], Vref);
}

/*************************
 *  @brief 获取右侧循迹模块电压
 *  @param 无
 *  @return 返回右侧循迹模块电压值，float类型
 *  @note 无
 *************************/
float Tcrt5000_Left(void)
{
    float Vref = 1.2 * (4095.0  / (float)adc_values[2]);
    float voltage = (adc_values[0] * Vref) / 4095.0f; // 将ADC值转换为电压（假设参考电压为3.3V）
    return voltage;
}

/*************************
 *  @brief 获取左侧循迹模块电压
 *  @param 无
 *  @return 返回左侧循迹模块电压值，float类型
 *  @note 无
 *************************/
float Tcrt5000_Right(void)
{
    float Vref = 1.2 * (4095.0  / (float)adc_values[2]);
    float voltage = (adc_values[1] * 3.3f) / 4095.0f; // 将ADC值转换为电压（假设参考电压为3.3V）
    return voltage;
}
