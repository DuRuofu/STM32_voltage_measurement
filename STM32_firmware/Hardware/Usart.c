#include "usart.h"
void MyUSART_Init()
{
	/* 定义GPIO和USART的结构体 */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* 使能GPIO和USART的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	/* 将USART TX（A9）的GPIO设置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/* 将USART RX（A10）的GPIO设置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/* 配置串口 */
	USART_InitStructure.USART_BaudRate=115200;										//波特率了设置为115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;	//不使用硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;						//使能接收和发送
	USART_InitStructure.USART_Parity=USART_Parity_No;								//不使用奇偶校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;							//1位停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;						//字长设置为8位
	USART_Init(USART1, &USART_InitStructure);										//初始化串口
	/* 使能串口 */
	USART_Cmd(USART1,ENABLE);
	
}

/* 发送一个字节 */
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/* 发送字符串 */
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k=0;
    do 
  	{
        Usart_SendByte( pUSARTx, *(str + k) );
      	k++;
  	} while(*(str + k)!='\0');
  	/* 等待发送完成 */
  	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  	{}
}

