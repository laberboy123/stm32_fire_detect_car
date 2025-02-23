#include "car.h"
#include "usart.h"
#include "tcrt5000.h"
#include "hc_sr04.h"
#include "stdbool.h"

#define OBSTACLE_THRESHOLD 15.0f  // ���� 15cm ����Ϊ�ϰ���
#define LINE_VOLTAGE    0.9f       // ��⵽���ߵ���ֵ��ѹ
//#define LINE_VOLTAGE_RIGHT 1.4f  // ������ 1.2V - 2.0
//#define LINE_VOLTAGE_LEFT  0.9f  // 0.8 - 1.2
#define KP          550 
#define UP_KP                460   // ����ϵ��
#define DOWN_KP                200   // ����ϵ��

// ��ǰģʽ����
// ��ʼΪѭ��ģʽ
CarMode currentMode = CONTROL_MODE;

// ����һ��ȫ�ֱ�������¼��ʱ������ʱ��
volatile uint32_t start_time = 0;
// ����һ��ȫ�ֱ���������Ƿ����ڱ���
volatile uint8_t is_alarming = 0;

/*************************
 *  @brief ���Ƶ�� IN1 PWM ռ�ձ�
 *  @param duty ռ�ձ�ֵ
 *  @return ��
 *  @note ��
 *************************/
static inline void __SetIN1PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN1_TIM, IN1_CH, duty);
}
/*************************
 *  @brief ���Ƶ�� IN2 PWM ռ�ձ�
 *  @param duty ռ�ձ�ֵ
 *  @return ��
 *  @note ��
 *************************/
static inline void __SetIN2PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN2_TIM, IN2_CH, duty);
}

/*************************
 *  @brief ���Ƶ�� IN3 PWM ռ�ձ�
 *  @param duty ռ�ձ�ֵ
 *  @return ��
 *  @note ��
 *************************/
static inline void __SetIN3PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN3_TIM, IN3_CH, duty);
}
/*************************
 *  @brief ���Ƶ�� IN4 PWM ռ�ձ�
 *  @param duty ռ�ձ�ֵ
 *  @return ��
 *  @note ��
 *************************/
static inline void __SetIN4PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN4_TIM, IN4_CH, duty);
}

/*************************
 *  @brief ������������ת
 *  @param mode ���ģʽ MotorModeö��
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Right_Motor(MotorMode mode, uint16_t speed)
{
    switch (mode) {
        case POSITIVE:
            // ��ת
            __SetIN1PWM(0);
            __SetIN2PWM(speed);
            break;
        case REVERSE:
            // ��ת
            __SetIN1PWM(speed);
            __SetIN2PWM(0);
            break;
        case STOP:
            // ֹͣ
            __SetIN1PWM(speed);
            __SetIN2PWM(speed);
            break;
    }
}

/*************************
 *  @brief ������������ת
 *  @param mode ���ģʽ MotorModeö��
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Left_Motor(MotorMode mode, uint16_t speed)
{
    switch (mode) {
        case POSITIVE:
            // ��ת
            __SetIN3PWM(speed);
            __SetIN4PWM(0);
            break;
        case REVERSE:
            // ��ת
            __SetIN3PWM(0);
            __SetIN4PWM(speed);
            break;
        case STOP:
            // ֹͣ
            __SetIN3PWM(speed);
            __SetIN4PWM(speed);
            break;
    }
}

/*************************
 *  @brief ����С��ǰ��
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Forward(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(POSITIVE, speed);
}

/*************************
 *  @brief ����С������
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Backward(uint16_t speed)
{
    Left_Motor(REVERSE, speed);
    Right_Motor(REVERSE, speed);
}

/*************************
 *  @brief С����ת��ң��ģʽ��
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Right(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(POSITIVE, 0);
}

/*************************
 *  @brief С����ת��ң��ģʽ��
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Left(uint16_t speed)
{
    Left_Motor(POSITIVE, 0);
    Right_Motor(POSITIVE, speed);
}

/*************************
 *  @brief С��ԭ�ص�ͷ
 *  @param speed ��̥ת��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Turnback(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(REVERSE, speed);
}

/*************************
 *  @brief ����С��ɲ��
 *  @param ��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Stop(void)
{
    Left_Motor(STOP, MAX_SPEED);
    Right_Motor(STOP, MAX_SPEED);
}

/*************************
 *  @brief ����С��ɲ��
 *  @param ��
 *  @return ��
 *  @note ��
 *************************/
void Motor_Slide(void)
{
    Left_Motor(STOP, 0);
    Right_Motor(STOP, 0);
}


/*************************
 *  @brief Զ�̿���С��
 *  @param Buffer�������ź�
 *  @return ��
 *  @note ��
 *************************/
void Motor_Control(uint8_t* Buffer)
{
    uint16_t speed = 600;
    
    if(strstr((char *)Buffer, "FORWARD"))
    {
        Motor_Forward(speed);
    }
    else if(strstr((char *)Buffer, "BACKWARD")) 
        Motor_Backward(speed);
    else if(strstr((char *)Buffer, "RIGHT"))
        Motor_Right(speed);
    else if(strstr((char *)Buffer, "LEFT")) 
        Motor_Left(speed);
    else 
        Motor_Stop();

     memset(Buffer, '\0', BufferLength);
}

/*************************
 *  @brief С����ѭ������Ϲ���
 *  @param ��
 *  @return ��
 *  @note ��
 *************************/
void Motor_TraceAndAvoid(void)
{
    // ����Ĭ���ٶ�
    uint16_t speed = 400;
    
    // ���� �͵�ƽ û����ߵ�ƽ
    // ��ȡ���Ҵ������ĵ�ѹ
    float right_voltage = Tcrt5000_Right() - 0.3f;
    float left_voltage = Tcrt5000_Left();
    
    // ��ȡ�������������ľ���
    float distance = Ultra_Distance();
    printf("left_voltage: %f ,right_voltage: %f ,distance: %f\n", left_voltage, right_voltage, distance);
    
    if (right_voltage + left_voltage < 2.6f)
    {
        Motor_Stop();
    }
    
    uint16_t left_speed = speed + (uint16_t)((left_voltage - 1.1) * KP);
    uint16_t right_speed = speed + (uint16_t)((right_voltage - 1.1) * KP);
    
    if(distance > OBSTACLE_THRESHOLD)
    {
        Left_Motor(POSITIVE, left_speed);
        Right_Motor(POSITIVE, right_speed);
//        printf("left_speed: %d ,left_speed: %d \n", left_speed, right_speed);
    }
    else
    {
        Motor_Stop();
    }
    
    
}


//void Motor_TraceAndAvoid(void)
//{
//    // ���� �͵�ƽ û����ߵ�ƽ
//    // ��ȡ���Ҵ������ĵ�ѹ
//    float right_voltage = Tcrt5000_Right() - 0.5f;
//    float left_voltage = Tcrt5000_Left();
//    
//    // ��ȡ�������������ľ���
//    float distance = Ultra_Distance();
//    printf("left_voltage: %f ,right_voltage: %f ,distance: %f\n", left_voltage, right_voltage, distance);
//    
//     // �ж������������Ƿ񶼼�⵽����
//    bool right_detected = (right_voltage > LINE_VOLTAGE);  // �ұ߼�⵽����
//    bool left_detected = (left_voltage > LINE_VOLTAGE);    // ��߼�⵽����
//    
//    // ��ѹ����㣨�ж��Ƿ�ƫ��켣��
//    float voltage_diff = right_voltage - left_voltage;
//    
//    // ����Ĭ���ٶ�
//    uint16_t speed = 570;

//    // �ж��Ƿ�ƫ��켣
//    // �ж��Ƿ�ǰ������ת����ת��ֹͣ
//    if (voltage_diff >-0.2 && voltage_diff < 0.2) // �����������������⵽���ߣ�ǰ��
//    {
//        Motor_Forward(speed);
//    } 
//    else if (voltage_diff > 0.2)          // ���ֻ���ұ߼�⵽���ߣ���ת
//    {
//        uint16_t right_speed = speed + (uint16_t)(voltage_diff * KP);
//        Left_Motor(POSITIVE,speed);
//        Right_Motor(POSITIVE, right_speed);
//        HAL_Delay(10);  // ת����ʱ
//    } 
//    else if (voltage_diff < -0.2)           // ���ֻ����߼�⵽���ߣ���ת
//    {
//        uint16_t left_speed = speed + (uint16_t)(voltage_diff * KP);
//        Left_Motor(POSITIVE,left_speed);
//        Right_Motor(POSITIVE, speed);
//        HAL_Delay(10);  // ת����ʱ
//    } 
//    else                              // ���û�м�⵽���ߣ�ֹͣ
//    {
//        Motor_Stop();
//    }

//    // ���ǰ�����ϰ��ֹͣ�����ˣ���ת�ƹ�
//    if (distance < OBSTACLE_THRESHOLD) {  // ����С����ֵ����Ϊǰ�����ϰ���
//        
//        __SetIN1PWM(0);
//        __SetIN2PWM(speed);     // ������ת

//        __SetIN3PWM(0);
//        __SetIN4PWM(speed);     // ���ַ�ת
//        HAL_Delay(50);

//        while(Tcrt5000_Right() + Tcrt5000_Right() < (2*LINE_VOLTAGE)) // һֱ��ͷֱ���Ҳ�ѭ��ģ���⵽����
//        {
//           Motor_Turnback(speed);
//        }
//        return;
//    }
//}

void Find_Fire(uint8_t* Buffer)
{
    if(strstr((char *)Buffer, "fire")) 
    {   
        printf("Find_Fire\n");
        start_time = HAL_GetTick(); // ��¼��ʼʱ��
        is_alarming = 1; // ������ڱ���
        Motor_Slide(); // ֹͣС��
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // ��������������
    }
    
}

void Fire_Warning(void)
{
    if(strstr((char *)UART1_RXBuffer, "fire")) 
    {   
        printf("Find_Fire\n");
        Motor_Slide(); // ֹͣС��
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // ��������������
        HAL_Delay(1000); // ��������
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // ֹͣ����
        printf("Warning_Stop\n");
        memset(UART1_RXBuffer , '\0' ,BufferLength);
    }
}

