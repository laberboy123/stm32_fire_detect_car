#include "car.h"
#include "usart.h"
#include "tcrt5000.h"
#include "hc_sr04.h"
#include "stdbool.h"

#define OBSTACLE_THRESHOLD 15.0f  // 假设 15cm 以下为障碍物
#define LINE_VOLTAGE    0.9f       // 检测到黑线的阈值电压
//#define LINE_VOLTAGE_RIGHT 1.4f  // 最大反射光 1.2V - 2.0
//#define LINE_VOLTAGE_LEFT  0.9f  // 0.8 - 1.2
#define KP          550 
#define UP_KP                460   // 加速系数
#define DOWN_KP                200   // 减速系数

// 当前模式变量
// 初始为循迹模式
CarMode currentMode = CONTROL_MODE;

// 定义一个全局变量来记录定时器启动时间
volatile uint32_t start_time = 0;
// 定义一个全局变量来标记是否正在报警
volatile uint8_t is_alarming = 0;

/*************************
 *  @brief 控制电机 IN1 PWM 占空比
 *  @param duty 占空比值
 *  @return 无
 *  @note 无
 *************************/
static inline void __SetIN1PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN1_TIM, IN1_CH, duty);
}
/*************************
 *  @brief 控制电机 IN2 PWM 占空比
 *  @param duty 占空比值
 *  @return 无
 *  @note 无
 *************************/
static inline void __SetIN2PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN2_TIM, IN2_CH, duty);
}

/*************************
 *  @brief 控制电机 IN3 PWM 占空比
 *  @param duty 占空比值
 *  @return 无
 *  @note 无
 *************************/
static inline void __SetIN3PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN3_TIM, IN3_CH, duty);
}
/*************************
 *  @brief 控制电机 IN4 PWM 占空比
 *  @param duty 占空比值
 *  @return 无
 *  @note 无
 *************************/
static inline void __SetIN4PWM(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(IN4_TIM, IN4_CH, duty);
}

/*************************
 *  @brief 控制左轮正反转
 *  @param mode 电机模式 MotorMode枚举
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Right_Motor(MotorMode mode, uint16_t speed)
{
    switch (mode) {
        case POSITIVE:
            // 正转
            __SetIN1PWM(0);
            __SetIN2PWM(speed);
            break;
        case REVERSE:
            // 反转
            __SetIN1PWM(speed);
            __SetIN2PWM(0);
            break;
        case STOP:
            // 停止
            __SetIN1PWM(speed);
            __SetIN2PWM(speed);
            break;
    }
}

/*************************
 *  @brief 控制右轮正反转
 *  @param mode 电机模式 MotorMode枚举
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Left_Motor(MotorMode mode, uint16_t speed)
{
    switch (mode) {
        case POSITIVE:
            // 正转
            __SetIN3PWM(speed);
            __SetIN4PWM(0);
            break;
        case REVERSE:
            // 反转
            __SetIN3PWM(0);
            __SetIN4PWM(speed);
            break;
        case STOP:
            // 停止
            __SetIN3PWM(speed);
            __SetIN4PWM(speed);
            break;
    }
}

/*************************
 *  @brief 控制小车前进
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Motor_Forward(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(POSITIVE, speed);
}

/*************************
 *  @brief 控制小车后退
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Motor_Backward(uint16_t speed)
{
    Left_Motor(REVERSE, speed);
    Right_Motor(REVERSE, speed);
}

/*************************
 *  @brief 小车右转（遥控模式）
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Motor_Right(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(POSITIVE, 0);
}

/*************************
 *  @brief 小车左转（遥控模式）
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Motor_Left(uint16_t speed)
{
    Left_Motor(POSITIVE, 0);
    Right_Motor(POSITIVE, speed);
}

/*************************
 *  @brief 小车原地掉头
 *  @param speed 轮胎转速
 *  @return 无
 *  @note 无
 *************************/
void Motor_Turnback(uint16_t speed)
{
    Left_Motor(POSITIVE, speed);
    Right_Motor(REVERSE, speed);
}

/*************************
 *  @brief 控制小车刹车
 *  @param 无
 *  @return 无
 *  @note 无
 *************************/
void Motor_Stop(void)
{
    Left_Motor(STOP, MAX_SPEED);
    Right_Motor(STOP, MAX_SPEED);
}

/*************************
 *  @brief 控制小车刹车
 *  @param 无
 *  @return 无
 *  @note 无
 *************************/
void Motor_Slide(void)
{
    Left_Motor(STOP, 0);
    Right_Motor(STOP, 0);
}


/*************************
 *  @brief 远程控制小车
 *  @param Buffer：控制信号
 *  @return 无
 *  @note 无
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
 *  @brief 小车的循迹与避障功能
 *  @param 无
 *  @return 无
 *  @note 无
 *************************/
void Motor_TraceAndAvoid(void)
{
    // 设置默认速度
    uint16_t speed = 400;
    
    // 反射 低电平 没反射高电平
    // 获取左右传感器的电压
    float right_voltage = Tcrt5000_Right() - 0.3f;
    float left_voltage = Tcrt5000_Left();
    
    // 获取超声波传感器的距离
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
//    // 反射 低电平 没反射高电平
//    // 获取左右传感器的电压
//    float right_voltage = Tcrt5000_Right() - 0.5f;
//    float left_voltage = Tcrt5000_Left();
//    
//    // 获取超声波传感器的距离
//    float distance = Ultra_Distance();
//    printf("left_voltage: %f ,right_voltage: %f ,distance: %f\n", left_voltage, right_voltage, distance);
//    
//     // 判断两个传感器是否都检测到黑线
//    bool right_detected = (right_voltage > LINE_VOLTAGE);  // 右边检测到黑线
//    bool left_detected = (left_voltage > LINE_VOLTAGE);    // 左边检测到黑线
//    
//    // 电压差计算（判断是否偏离轨迹）
//    float voltage_diff = right_voltage - left_voltage;
//    
//    // 设置默认速度
//    uint16_t speed = 570;

//    // 判断是否偏离轨迹
//    // 判断是否前进、左转、右转或停止
//    if (voltage_diff >-0.2 && voltage_diff < 0.2) // 如果两个传感器都检测到黑线，前进
//    {
//        Motor_Forward(speed);
//    } 
//    else if (voltage_diff > 0.2)          // 如果只有右边检测到黑线，左转
//    {
//        uint16_t right_speed = speed + (uint16_t)(voltage_diff * KP);
//        Left_Motor(POSITIVE,speed);
//        Right_Motor(POSITIVE, right_speed);
//        HAL_Delay(10);  // 转向延时
//    } 
//    else if (voltage_diff < -0.2)           // 如果只有左边检测到黑线，右转
//    {
//        uint16_t left_speed = speed + (uint16_t)(voltage_diff * KP);
//        Left_Motor(POSITIVE,left_speed);
//        Right_Motor(POSITIVE, speed);
//        HAL_Delay(10);  // 转向延时
//    } 
//    else                              // 如果没有检测到黑线，停止
//    {
//        Motor_Stop();
//    }

//    // 如果前方有障碍物，停止，后退，右转绕过
//    if (distance < OBSTACLE_THRESHOLD) {  // 距离小于阈值，认为前方有障碍物
//        
//        __SetIN1PWM(0);
//        __SetIN2PWM(speed);     // 左轮正转

//        __SetIN3PWM(0);
//        __SetIN4PWM(speed);     // 右轮反转
//        HAL_Delay(50);

//        while(Tcrt5000_Right() + Tcrt5000_Right() < (2*LINE_VOLTAGE)) // 一直掉头直到右侧循迹模块检测到黑线
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
        start_time = HAL_GetTick(); // 记录开始时间
        is_alarming = 1; // 标记正在报警
        Motor_Slide(); // 停止小车
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 启动蜂鸣器报警
    }
    
}

void Fire_Warning(void)
{
    if(strstr((char *)UART1_RXBuffer, "fire")) 
    {   
        printf("Find_Fire\n");
        Motor_Slide(); // 停止小车
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 启动蜂鸣器报警
        HAL_Delay(1000); // 持续报警
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // 停止报警
        printf("Warning_Stop\n");
        memset(UART1_RXBuffer , '\0' ,BufferLength);
    }
}

