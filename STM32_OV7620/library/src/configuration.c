                                          /****************************************************************************************
* 文件名：bsp.c
* 功能：初始化系统,板级硬件驱动
* 作者：石楚雄
* 日期：2012.02.26
* 备注：进主函数后首先调用次函数
****************************************************************************************/

#include"configuration.h"
#include <stm32f10x_lib.h>


//定义HSE启动状态
ErrorStatus HSEStartUpStatus;

/****************************************************************************
* 名称：ConfigInit()
* 功能：初始化系统
* 入口参数：无
* 出口参数：无
****************************************************************************/
void ConfigInit(void)
{
	RCC_Configuration();	   //系统时钟配置（72MHZ）
	NVIC_Configuration();	   //嵌套向量中断优先级配置
	GPIO_Configuration();	   //IO口模式及方向配置
	EXTI_Configuration();	   //定时器主要用于输入捕获和PWM输出
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
	//定义NVIC初始化结构体
	NVIC_InitTypeDef NVIC_InitStructure;
	//设置中断向量地址
	NVIC_DeInit();
	#ifdef  VECT_TAB_RAM  
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    //NVIC_InitStructure.NVIC_IRQChannel = 中断通道名;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
    //抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	//启动此通道的中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //中断初始化
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
    //抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    //响应优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;

	NVIC_Init(&NVIC_InitStructure);
}


void EXTI_Configuration(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;

	EXTI_DeInit();	  //将EXTI寄存器设置为缺省值	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;	//	  中断通道为0通道
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	 //	 外部模式为中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;// 下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	//EXTI_Line的中断使能
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

  	USART_InitStructure.USART_BaudRate = 115200; // 设置了USART传输的波特率 ；
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	// 8位数据 ；
  	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 在帧结尾传输1个停止位 ；
  	USART_InitStructure.USART_Parity = USART_Parity_No ; // 奇偶失能 ；
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// RTS和CTS失能 ；
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 接收发送使能 ；
	USART_Init(USART1, &USART_InitStructure); // 根据USART_InitStruct中指定的参数初始化外设USART2寄存器 ；
	
	USART_Cmd(USART1, ENABLE); // 使能USART2外设 ；
}

void EXTI_Disable(void)
{
    EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;	//EXTI_Line的中断使能
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI_Enable(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	//EXTI_Line的中断使能
	EXTI_Init(&EXTI_InitStructure);
}

