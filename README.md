# STM32 + K210 火焰检测小车
## 硬件组成
- STM32F103C8T6
- K210
- HC-SR04
- TCRT5000
- AI-WB2-01S
- DRV8833
- 蜂鸣器
- 电机、小车底盘
## 文件简述
- HARDWARE：模块代码
- FrieWaringCar.ioc：CubeMX 工程文件
- Core：STM32片上外设代码，包含 main.c
- Drivers：HAL 驱动库
- MDK-ARM：keil 项目文件
