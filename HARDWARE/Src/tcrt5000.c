#include "tcrt5000.h"
#include "adc.h"
#include "usart.h"

#define ADC_BUF_SIZE 3

uint16_t adc_values[ADC_BUF_SIZE]; // ���ڴ洢 PA6��PA7���ڲ��ο���ѹ
float voltage[2] = {0.0};

/*************************
 *  @brief ���� ADC ��ѹת��
 *  @param ��
 *  @return ��
 *  @note ��
 *************************/
void Start_ADC_Conversion(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);    // У׼ADC
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, ADC_BUF_SIZE); // ����ADC
}


/*************************
 *  @brief ��� ADC ���ĵ�ѹֵ
 *  @param ��
 *  @return ��
 *  @note ��
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
 *  @brief ��ȡ�Ҳ�ѭ��ģ���ѹ
 *  @param ��
 *  @return �����Ҳ�ѭ��ģ���ѹֵ��float����
 *  @note ��
 *************************/
float Tcrt5000_Left(void)
{
    float Vref = 1.2 * (4095.0  / (float)adc_values[2]);
    float voltage = (adc_values[0] * Vref) / 4095.0f; // ��ADCֵת��Ϊ��ѹ������ο���ѹΪ3.3V��
    return voltage;
}

/*************************
 *  @brief ��ȡ���ѭ��ģ���ѹ
 *  @param ��
 *  @return �������ѭ��ģ���ѹֵ��float����
 *  @note ��
 *************************/
float Tcrt5000_Right(void)
{
    float Vref = 1.2 * (4095.0  / (float)adc_values[2]);
    float voltage = (adc_values[1] * 3.3f) / 4095.0f; // ��ADCֵת��Ϊ��ѹ������ο���ѹΪ3.3V��
    return voltage;
}
