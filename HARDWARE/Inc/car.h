#ifndef CAR_H
#define CAR_H

#include "tim.h"

#define IN1_TIM &htim4
#define IN2_TIM &htim4
#define IN3_TIM &htim2
#define IN4_TIM &htim2


// IN1 连接 PB9
#define IN1_CH TIM_CHANNEL_4 

// IN2 连接 PB8
#define IN2_CH TIM_CHANNEL_3

// IN3 连接 PA0
#define IN3_CH TIM_CHANNEL_1

// IN4 连接 PA1
#define IN4_CH TIM_CHANNEL_2

// IN1 IN2 通过 TIM4 CH4 CH3 控制，右轮
// IN3 IN4 通过 TIM2 CH1 CH2 控制，左轮

#define MAX_SPEED 1000

// 定义模式枚举
typedef enum {
    CONTROL_MODE,  // 遥控模式
    TRACE_MODE     // 循迹模式
} CarMode;

typedef enum {
    POSITIVE,   // 正向
    REVERSE,    // 反向
    STOP        // 停止
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
