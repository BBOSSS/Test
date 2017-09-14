#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

void RCC_Configuration(void);	   //系统时钟配置（72MHZ）
void NVIC_Configuration(void);	   //嵌套向量中断优先级配置
void GPIO_Configuration(void);	   //IO口模式及方向配置
void EXTI_Configuration(void);	   //定时器主要用于输入捕获和PWM输出
void USART_Configuration(void);
void ConfigInit(void);
void EXTI_Disable(void);
void EXTI_Enable(void);
#endif
