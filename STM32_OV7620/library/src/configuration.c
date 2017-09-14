                                          /****************************************************************************************
* �ļ�����bsp.c
* ���ܣ���ʼ��ϵͳ,�弶Ӳ������
* ���ߣ�ʯ����
* ���ڣ�2012.02.26
* ��ע���������������ȵ��ôκ���
****************************************************************************************/

#include"configuration.h"
#include <stm32f10x_lib.h>


//����HSE����״̬
ErrorStatus HSEStartUpStatus;

/****************************************************************************
* ���ƣ�ConfigInit()
* ���ܣ���ʼ��ϵͳ
* ��ڲ�������
* ���ڲ�������
****************************************************************************/
void ConfigInit(void)
{
	RCC_Configuration();	   //ϵͳʱ�����ã�72MHZ��
	NVIC_Configuration();	   //Ƕ�������ж����ȼ�����
	GPIO_Configuration();	   //IO��ģʽ����������
	EXTI_Configuration();	   //��ʱ����Ҫ�������벶���PWM���
	USART_Configuration();
}

void RCC_Configuration(void)
{
	/* RCC system reset(for debug purpose) */
  	RCC_DeInit();

  	/* Enable HSE */
  	RCC_HSEConfig(RCC_HSE_ON);

  	/* Wait till HSE is ready */
  	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08);

  	/* Enable GPIOA, GPIOC, ADC1 and TIM1 clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);

  	/* Enable USART2 clocks */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	//����NVIC��ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStructure;
	//�����ж�������ַ
	NVIC_DeInit();
	#ifdef  VECT_TAB_RAM  
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    //NVIC_InitStructure.NVIC_IRQChannel = �ж�ͨ����;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
    //��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    //��Ӧ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	//������ͨ�����ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //�жϳ�ʼ��
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
    //��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    //��Ӧ���ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;

	NVIC_Init(&NVIC_InitStructure);
}


void EXTI_Configuration(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;

	EXTI_DeInit();	  //��EXTI�Ĵ�������Ϊȱʡֵ	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;	//	  �ж�ͨ��Ϊ0ͨ��
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	 //	 �ⲿģʽΪ�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;// �½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	//EXTI_Line���ж�ʹ��
	EXTI_Init(&EXTI_InitStructure);

}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	/* Configure USART2 RTS (PA.01) and USART2 Tx (PA.02) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Configure USART2 CTS (PA.00) and USART2 Rx (PA.03) as input floating */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 ;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);
}

void USART_Configuration(void)
{
  	USART_InitTypeDef USART_InitStructure;

  	USART_InitStructure.USART_BaudRate = 115200; // ������USART����Ĳ����� ��
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	// 8λ���� ��
  	USART_InitStructure.USART_StopBits = USART_StopBits_1; // ��֡��β����1��ֹͣλ ��
  	USART_InitStructure.USART_Parity = USART_Parity_No ; // ��żʧ�� ��
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// RTS��CTSʧ�� ��
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// ���շ���ʹ�� ��
	USART_Init(USART1, &USART_InitStructure); // ����USART_InitStruct��ָ���Ĳ�����ʼ������USART2�Ĵ��� ��
	
	USART_Cmd(USART1, ENABLE); // ʹ��USART2���� ��
}

void EXTI_Disable(void)
{
    EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;	//EXTI_Line���ж�ʹ��
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI_Enable(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	//EXTI_Line���ж�ʹ��
	EXTI_Init(&EXTI_InitStructure);
}

