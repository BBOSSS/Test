#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

void RCC_Configuration(void);	   //ϵͳʱ�����ã�72MHZ��
void NVIC_Configuration(void);	   //Ƕ�������ж����ȼ�����
void GPIO_Configuration(void);	   //IO��ģʽ����������
void EXTI_Configuration(void);	   //��ʱ����Ҫ�������벶���PWM���
void USART_Configuration(void);
void ConfigInit(void);
void EXTI_Disable(void);
void EXTI_Enable(void);
#endif
