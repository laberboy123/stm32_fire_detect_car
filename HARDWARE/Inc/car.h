#ifndef CAR_H
#define CAR_H

#include "tim.h"

#define IN1_TIM &htim4
#define IN2_TIM &htim4
#define IN3_TIM &htim2
#define IN4_TIM &htim2


// IN1 ���� PB9
#define IN1_CH TIM_CHANNEL_4 

// IN2 ���� PB8
#define IN2_CH TIM_CHANNEL_3

// IN3 ���� PA0
#define IN3_CH TIM_CHANNEL_1

// IN4 ���� PA1
#define IN4_CH TIM_CHANNEL_2

// IN1 IN2 ͨ�� TIM4 CH4 CH3 ���ƣ�����
// IN3 IN4 ͨ�� TIM2 CH1 CH2 ���ƣ�����

#define MAX_SPEED 1000

// ����ģʽö��
typedef enum {
    CONTROL_MODE,  // ң��ģʽ
    TRACE_MODE     // ѭ��ģʽ
} CarMode;

typedef enum {
    POSITIVE,   // ����
    REVERSE,    // ����
    STOP        // ֹͣ
} MotorMode;

extern CarMode currentMode;

extern volatile uint32_t start_time;
extern volatile uint8_t is_alarming;

void Left_Motor(MotorMode mode, uint16_t speed);
void Right_Motor(MotorMode mode, uint16_t speed);

void Motor_Forward(uint16_t speed);
void Motor_Backward(uint16_t speed);
void Motor_Right(uint16_t speed);
void Motor_Left(uint16_t speed);
void Motor_Turnback(uint16_t speed);
void Motor_Stop(void);
void Motor_Slide(void);
void Motor_Control(uint8_t* Buffer);
void Motor_TraceAndAvoid(void);
void Find_Fire(uint8_t* Buffer);
void Fire_Warning(void);

#endif
