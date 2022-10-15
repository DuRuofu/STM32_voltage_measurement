#include "stm32f10x.h" // Device header
#include "stdio.h"
#include "Delay.h"
#include "OLED.h"
#include "ADC.h"
#include "Usart.h"
u16 USART_RX_STA = 0;		 //接收状态标记
uint16_t ADValue1, ADValue2; //保存原始值
float Voltage1, Voltage2;	 //保存换算后的值
char str[80];				 //转化字符串临时变量
int main(void)
{
	OLED_Init();	//初始化屏幕
	AD_Init();		//初始化ADC
	MyUSART_Init(); //初始化串口
	//屏幕默认显示格式
	OLED_ShowString(1, 1, "ADValue1");
	OLED_ShowString(2, 1, "Voltage1:0.00V");
	OLED_ShowString(3, 1, "ADValue2");
	OLED_ShowString(4, 1, "Voltage2:0.00V");
	while (1)
	{
		//获取通道采集的值
		ADValue1 = AD_GgtValue(ADC_Channel_0);
		ADValue2 = AD_GgtValue(ADC_Channel_1);
		//进行换算，换算为0~3.3V的电压值
		Voltage1 = (float)ADValue1 / 4095 * 3.3;
		Voltage2 = (float)ADValue2 / 4095 * 3.3;
		//显示通道一数据
		OLED_ShowNum(1, 10, ADValue1, 4);						  //采集的原始值
		OLED_ShowNum(2, 10, Voltage1, 1);						  //整数位
		OLED_ShowNum(2, 12, (uint16_t)(Voltage1 * 100) % 100, 2); //小数位
		//显示通道二数据
		OLED_ShowNum(3, 10, ADValue2, 4);
		OLED_ShowNum(4, 10, Voltage2, 1);
		OLED_ShowNum(4, 12, (uint16_t)(Voltage2 * 100) % 100, 2);
		//串口发送数据
		sprintf(str, "ADValue1: %4d   ", ADValue1);	   //将采集的原始值转化为字符串
		Usart_SendString(USART1, str);				   //发送
		sprintf(str, "Voltage1 = %f V\r\n", Voltage1); //将转化后的电压值转化为字符串
		Usart_SendString(USART1, str);
		sprintf(str, "ADValue2: %4d   ", ADValue2);
		Usart_SendString(USART1, str);
		sprintf(str, "Voltage2 = %f V\r\n", Voltage2);
		Usart_SendString(USART1, str);
		Usart_SendString(USART1, "\r\n"); //空行
		Delay_ms(500); //延时屏幕刷新,限制串口发送速度
	}
}
