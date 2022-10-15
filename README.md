## 技术方案（固件开发）

时间：2022年6月12日

**目录：**

[toc]

### 一、需求描述：

> 使用STM32或ESP32任意型号， 搭建环境。 使用C或python编程， 实现用STM32或ESP32上集成的AD， 分别采集2个不同型号干电池的电压信号， 将采集到的电压数据通过网口或串口传输到上位机（笔记本电脑） 并在D盘根目录保存成一个TXT文件。  

### 二、需求分析：

#### 2.1.采集2个不同型号干电池的电压信号

需求为使用单片机内置AD转换器来采集干电池电压信号，这里使用型号为STM32F103C8T6的单片机进行实操。其内置了两个ADC（Analog-Digital Converter）模拟-数字转换器，分别是ADC1、ADC2，以及10个外部输入通道。ADC可以将引脚上连续变化的模拟电压转换为内存中存储的数字变量，建立起模拟电路到数字电路的桥梁，通过ADC我们可以直接通过单片机引脚输入来测量电压值，不需要外接任何外部设备。测量电压值的范围为0~3.3V（与单片机供电电压一致），当然也可通过设计外围分压电路来增大测量范围。

#### 2.2.采集到的电压数据通过网口或串口传输

需求为使用单片机来传输数据到上位机，这里采用串口传输的方式进行，使用STM32内置的USART（通用同步异步收发器）实现。由于笔记本没有自带的串口接口，这里还需要用到一个USB转串口的模块，以便把USB传输过来的电平转换为TTL电平，TTL电平才能与串口调试助手建立联系。

#### 2.3.上位机接收数据并保存

上位机端比较灵活，只需要获取电脑串口受到的信息，并进行保存即可。可以通过QT,MFC等开发平台或者直接使用python的serial库，底层的C语言都可以进行上位机的编写。这里使用基于对话框的MFC编程进行上位机的开发，主要是由于个人对MFC开发有稍微了解，并且MFC的可视化界面更加直观清楚的看到上位机效果。

### 三、系统框架（系统原理）

#### 3.1.系统总体思路与框架

系统总体可简单分为单片机部分和电脑上位机部分。
单片机部分主要调用STM32F103C8T6的ADC资源和USART资源，来实现电压值的读取和发送。
电脑上位机部分则主要通过MFC程序接收串口发送的数据并进行保存等操作。

##### 3.1.1系统架构框图

<img src="https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1654998679874.png" style="zoom:67%;" />

##### 3.1.2系统流程框图

<img src="https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1654998722428.png" style="zoom:67%;" />

#### 3.2.子系统分析

##### 3.2.1单片机部分

主要使用到的片上资源为ADC资源和USART资源，分别用来测量引脚电压值和发送电压信息。同时本次历程还使用外置的OLED显示屏来显示调试信息。

<img src="https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1654998633808.png" style="zoom: 67%;" />

单片机内置的ADC结构如下图所示

<img src="https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1654999519065.png" style="zoom:67%;" />

其有最基本的四种转换模式：1.单次转换，非扫描模式  2.连续转换，非扫描模式  3.单次转换，扫描模式 4.连续转换，扫描模式分别对应不同的工作状态。
在此由于只需要采集两路信号，所以我使用单次转换，非扫描模式 通过在每次扫描的间隙改变选择的规则组输入通道来实现双通道AD采集。

##### 3.2.2电脑上位机部分

win32下对串口的操作可以通过两种方式：**ActiveX**控件和**Windows API**函数，第一种程序简单但是欠缺灵活，第二种自由灵活编程不易。由于本人对上位机部分了解不多，故采用较为简单的**ActiveX**控件来开发上位机。

### 四、需求实现（代码）

#### 4.1.单片机部分

单片机部分代码均基于标准库开发，开发工具为keil5 ARM 以及VS Code(Keil Assistant扩展)。

##### 4.1.1 ADC采集

ADC.C文件：

```c
#include "stm32f10x.h" // Device header
void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC时钟配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //配置GPIO为模拟输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//设置两个通道0和1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); //选择规则组输入通道
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC配置
    ADC_Cmd(ADC1, ENABLE); //开启ADC
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
        ; //等待复位校准完成
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
        ; //等待校准完成
}

uint16_t AD_GgtValue(uint8_t ADC_Channel)//获取结果
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);//指定ADC_Channel通道进行转换(选择规则组输入通道)
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);//等待转换完成
    return ADC_GetConversionValue(ADC1);//获取值并返回
}

```

`AD_Init(void)`是ADC初始化函数，包括最初的时钟使能，GPIO配置，ADC资源配置，初始校准等工作。这里设置 `GPIO_Pin_0|GPIO_Pin_1`为数据输入引脚，对应单片机开发板的A0和A1引脚。`ADC_Cmd(ADC1, ENABLE); //开启ADC`将ADC设置为软件触发，以便于下面 `AD_GgtValue(uint8_t ADC_Channel)`函数里使用 `ADC_SoftwareStartConvCmd(ADC1, ENABLE);`来触发ADC采样。

 `AD_GgtValue(uint8_t ADC_Channel)//获取结果`用于获取单次采样的结果，输入的参数`ADC_Channel`为指定的通道。返回值`ADC_GetConversionValue(ADC1);`是ADC采样的原始数据，为：0~4095， 对应测量范围：0-3.3V。

ADC.h文件：

```c
#ifndef __ADC_H
#define __ADC_H
void AD_Init(void);
uint16_t AD_GgtValue(uint8_t ADC_Channel);
#endif

```

##### 4.1.2串口通讯部分：

Usart.c文件

```c
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
	USART_InitStructure.USART_BaudRate=115200;										 //波特率了设置为115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;	    //不使用硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;						  //使能接收和发送
	USART_InitStructure.USART_Parity=USART_Parity_No;								 //不使用奇偶校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;							 //1位停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;						 //字长设置为8位
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

```

 `MyUSART_Init()`是串口初始化函数，使能GPIO和USART的时钟，将USART TX（A9）的GPIO设置为推挽复用模式，用于发送数据。将USART RX（A10）的GPIO设置为浮空输入模式用于接收数据。配置串口波特率为115200，不使用奇偶校验位，1位停止位，8位字长。

`Usart_SendByte`（）通过调用`USART_SendData(pUSARTx,ch);`标准库函数来 发送一个字节数据到USART。`Usart_SendString( USART_TypeDef * pUSARTx, char *str)`在`Usart_SendByte`的基础上实现发送字符串的功能。

Usart.h文件

```c
#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"
void MyUSART_Init(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
#endif

```

##### 4.1.3主函数部分：

main.c文件：

```c
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

```

主函数中还调用了`#include "Delay.h"`用于延时  ，`#include "OLED.h"`用于使用`OLED_ShowNum（）`和`OLED_ShowString（）`来在外围显示屏展示调试信息（外接于B8.B9引脚），`#include "stdio.h"`用于调用`sprintf()`将数值转换为字符串发送.

分布讲解:

```c
	OLED_Init();	//初始化屏幕
	AD_Init();		//初始化ADC
	MyUSART_Init(); //初始化串口
	//屏幕默认显示格式
	OLED_ShowString(1, 1, "ADValue1");
	OLED_ShowString(2, 1, "Voltage1:0.00V");
	OLED_ShowString(3, 1, "ADValue2");
	OLED_ShowString(4, 1, "Voltage2:0.00V");
```

进行初始化.

```c
		ADValue1 = AD_GgtValue(ADC_Channel_0);
		ADValue2 = AD_GgtValue(ADC_Channel_1);
```

是对ADC_Channel_0,ADC_Channel_1两个通道进行信息采集.

```c
		Voltage1 = (float)ADValue1 / 4095 * 3.3;
		Voltage2 = (float)ADValue2 / 4095 * 3.3;
```

采集后的信息换算为电压值.

```c
		//显示通道一数据
		OLED_ShowNum(1, 10, ADValue1, 4);						  //采集的原始值
		OLED_ShowNum(2, 10, Voltage1, 1);						  //整数位
		OLED_ShowNum(2, 12, (uint16_t)(Voltage1 * 100) % 100, 2); //小数位
		//显示通道二数据
		OLED_ShowNum(3, 10, ADValue2, 4);
		OLED_ShowNum(4, 10, Voltage2, 1);
		OLED_ShowNum(4, 12, (uint16_t)(Voltage2 * 100) % 100, 2);
```

OLED进行显示,以便于调试.

```c
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
```

将数字格式化后通过串口发送.

##### 4.1.4效果展示：

<img src="https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/IMG_20220612_105746.jpg" style="zoom:67%;" />

OIED显示屏可以正常显示adc采集的原始值和转换的电压值。串口测试工具XCOM串口助手也可以正常的接收到数据。

#### 4.2.基于FMC开发的上位机

上位机开发采用MFC编程开发，平台为VS2010。参考文档：https://www.cnblogs.com/codecamel/p/5075675.html

##### 4.2.1界面效果如下：

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1655003317960.png)

![image-20220612114059820](C:\Users\xin\AppData\Roaming\Typora\typora-user-images\image-20220612114059820.png)

##### 4.2.2变量绑定情况：

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1655003425299.png)

##### 4.2.3各控件事件代码：

###### 设置各下拉框的初始数据

```c
BOOL CUpperMachineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_EditTxData="此功能不可用！";
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: Add extra initialization here
	AddCom();//向组合框中添加串口设备	
	m_COM.SetCurSel(1);	//设置默认的选项
	//...

	/*//COM口
	m_COM.AddString(TEXT("COM1"));
	m_COM.AddString(TEXT("COM2"));
	m_COM.AddString(TEXT("COM3"));
	m_COM.AddString(TEXT("COM4"));
	m_COM.AddString(TEXT("COM5"));
	m_COM.AddString(TEXT("COM6"));
	m_COM.AddString(TEXT("COM7"));
	m_COM.AddString(TEXT("COM8"));
	m_COM.AddString(TEXT("COM9"));
	m_COM.AddString(TEXT("COM10"));
	m_COM.AddString(TEXT("COM11"));
	m_COM.AddString(TEXT("COM12"));
	m_COM.AddString(TEXT("COM13"));
	//设置默认的选项
	m_COM.SetCurSel(5);*/

	//波特率
	m_BaudRate.AddString(TEXT("1382400"));
	m_BaudRate.AddString(TEXT("921600"));
	m_BaudRate.AddString(TEXT("460800"));
	m_BaudRate.AddString(TEXT("256000"));
	m_BaudRate.AddString(TEXT("230400"));
	m_BaudRate.AddString(TEXT("128000"));
	m_BaudRate.AddString(TEXT("115200"));
	m_BaudRate.AddString(TEXT("76800"));
	m_BaudRate.AddString(TEXT("57600"));
	m_BaudRate.AddString(TEXT("43000"));
	m_BaudRate.AddString(TEXT("38400"));
	m_BaudRate.AddString(TEXT("19200"));
	m_BaudRate.AddString(TEXT("14400"));
	m_BaudRate.AddString(TEXT("9600"));
	m_BaudRate.AddString(TEXT("4800"));
	m_BaudRate.AddString(TEXT("2400"));
	m_BaudRate.AddString(TEXT("1200"));

	//设置默认的选项
	m_BaudRate.SetCurSel(0);

	//停止位
	m_StopBit.AddString(TEXT("1"));
	m_StopBit.AddString(TEXT("1.5"));
	m_StopBit.AddString(TEXT("2"));

	//设置默认的选项
	m_StopBit.SetCurSel(0);

	//数据位
	m_DataBit.AddString(TEXT("8"));
	m_DataBit.AddString(TEXT("7"));
	m_DataBit.AddString(TEXT("6"));
	m_DataBit.AddString(TEXT("5"));

	//设置默认的选项
	m_DataBit.SetCurSel(3);

	m_Parity.AddString(TEXT("无"));
	m_Parity.AddString(TEXT("奇校检"));
	m_Parity.AddString(TEXT("偶校验"));
	//设置默认的选项
	m_Parity.SetCurSel(2);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
```

###### 动态可用串口获取：（[mfc开发串口，自动获取可用串口，采用注册表方式](https://blog.csdn.net/qq_40305944/article/details/104533969?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522165500312916781432930256%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=165500312916781432930256&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-2-104533969-null-null.nonecase&utm_term=mfc&spm=1018.2226.3001.4450)）

```c

//获取串口配置
CString CUpperMachineDlg::GetUsartAllcation()
{
	//拿到波特率的索引
	int BaudRate = m_BaudRate.GetCurSel();
	CString BaudRate_str;
	m_BaudRate.GetLBText(BaudRate, BaudRate_str);//根据索引获取内容
//	MessageBox(BaudRate_str);

	//拿到停止位的索引
	int StopBit = m_StopBit.GetCurSel();
	CString StopBit_str;
	m_StopBit.GetLBText(StopBit, StopBit_str);//根据索引获取内容
//	MessageBox(StopBit_str);

	//拿到拿到数据位的索引
	int DataBit = m_DataBit.GetCurSel();
	CString DataBit_str;
	m_DataBit.GetLBText(DataBit, DataBit_str);//根据索引获取内容
//	MessageBox(DataBit_str);

	//拿到拿到奇偶校验位的索引
	int Parity = m_Parity.GetCurSel();
	char Parity_data;
	switch (Parity)//根据索引设置校检位
	{
	case 0: Parity_data = 'E';//无
		break;
	case 1: Parity_data = 'O';//奇校检
		break;
	case 2: Parity_data = 'N';//偶校检
		break;
	default:
		break;
	}
//	MessageBox(Parity_str);

	//合并字符串
	CString str;
	str.Format(_T("%s,%c,%s,%s"), BaudRate_str, Parity_data, DataBit_str, StopBit_str);
//	MessageBox(str);

	return str;
}

BOOL CUpperMachineDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	//DEV_BROADCAST_DEVICEINTERFACE* dbd = (DEV_BROADCAST_DEVICEINTERFACE*) dwData;
	switch (nEventType)
	{
	case DBT_DEVICEREMOVECOMPLETE://移除设备
	case DBT_DEVICEARRIVAL://添加设备
		RefreshCom();//刷新组合框的内容
		break;

	default:
		break;
	}

	return TRUE;

}
void CUpperMachineDlg::RefreshCom(void)
{
	int count = m_COM.GetCount();

	for (int i = 0; i < count; i++)
	{
		m_COM.DeleteString(count - 1 - i);
	}
	AddCom();
	m_COM.SetCurSel(0);
}

void CUpperMachineDlg::EnumerateSerialPorts(CUIntArray& ports, CUIntArray& portse, CUIntArray& portsu)
{
	//清除串口数组内容
	ports.RemoveAll();//所有存在串口
	portse.RemoveAll();//可用串口
	portsu.RemoveAll();//已占用串口

	//因为至多有255个串口，所以依次检查各串口是否存在
	//如果能打开某一串口，或打开串口不成功，但返回的是 ERROR_ACCESS_DENIED错误信息，
	//都认为串口存在，只不过后者表明串口已经被占用
	//否则串口不存在
	for (int i = 1; i < 256; i++)
	{
		//Form the Raw device name
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"), i);

		//Try to open the port
		BOOL bSuccess = FALSE;
		HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (hPort == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();

			if (dwError == ERROR_ACCESS_DENIED)
			{
				bSuccess = TRUE;
				portsu.Add(i);       //已占用的串口
			}
		}
		else
		{
			//The port was opened successfully
			bSuccess = TRUE;
			portse.Add(i);      //可用的串口
			//Don't forget to close the port, since we are going to do nothing with it anyway
			CloseHandle(hPort);
		}

		//Add the port number to the array which will be returned
		if (bSuccess)
			ports.Add(i);   //所有存在的串口
	}
}

void CUpperMachineDlg::AddCom(void)
{

	EnumerateSerialPorts(ports, portse, portsu);
	unsigned short uicounter;
	unsigned short uisetcom;
	CString str;

	//获取可用串口个数
	uicounter = portse.GetSize();
	//如果个数大于0
	if (uicounter > 0)
	{
		//初始化串口列表框
		for (int i = 0; i < uicounter; i++)
		{
			uisetcom = portse.ElementAt(i);
			str.Format(_T("COM%d "), uisetcom);
			m_COM.AddString(str);
		}
	}
}
```

###### 串口处理程序：

```c

void CUpperMachineDlg::OnCommMscomm1()//串口处理程序
{
	// TODO:  在此处添加消息处理程序代码
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_Comm1.get_CommEvent() == 2) //事件值为2表示接收缓冲区内有字符
	{             
		variant_inp = m_Comm1.get_Input(); //读缓冲区
		safearray_inp = variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
		len = safearray_inp.GetOneDimSize(); //得到有效数据长度
		for (k = 0; k < len; k++)
			safearray_inp.GetElement(&k, rxdata + k);//转换为BYTE型数组
		for (k = 0; k < len; k++) //将数组转换为Cstring型变量
		{
			BYTE bt = *(char*)(rxdata + k); //字符型
			strtemp.Format(_T("%c"), bt); //将字符送入临时变量strtemp存放
			m_EditRxData += strtemp; //加入接收编辑框对应字符串 
		}
	}
	//UpdateData(FALSE); //更新编辑框内容
	SetDlgItemText(IDC_EditRxData, m_EditRxData);//不使用UpdateData(FALSE);的原因是该函数会
	//刷新整个对话框的数据，而SetDlgItemText()
	//只更新接收编辑框的数据。
}
```

###### 打开串口按钮点击事件函数：

```c
void CUpperMachineDlg::OnBnClickedBtnopen()//打开串口
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Comm1.get_PortOpen())//获取串口工作状态，为0代表代开
	{
		m_Comm1.put_PortOpen(FALSE);//如果已经打开，就关闭串口
		m_BtnOpen.SetWindowText(TEXT("打开串口"));//将打开串口按钮换成关闭串口
	}
	else
	{
/*		//拿到COM口索引
		int Com = m_COM.GetCurSel()+1;
		m_Comm1.put_CommPort(Com); //选择com，可根据具体情况更改*/


		//当前端口号
		int curPort = portse.ElementAt(m_COM.GetCurSel());//根据索引找到对应的com口编号
		m_Comm1.put_CommPort(curPort);//端口号

		m_Comm1.put_InBufferSize(1024); //设置输入缓冲区的大小，Bytes
		m_Comm1.put_OutBufferSize(1024); //设置输入缓冲区的大小，Bytes//

		CString Usart_Allcation = GetUsartAllcation();//获取串口参数配置
		m_Comm1.put_Settings(Usart_Allcation); //波特率9600，无校验，8个数据位，1个停止位
		m_Comm1.put_InputMode(1); //1：表示以二进制方式检取数据
		m_Comm1.put_RThreshold(1);
		//参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
		m_Comm1.put_InputLen(0); //设置当前接收区数据长度为0

		m_Comm1.put_PortOpen(TRUE);//打开串口
		m_BtnOpen.SetWindowText(TEXT("关闭串口"));//将打开串口按钮换成关闭串口

		m_Comm1.get_Input();//先预读缓冲区以清除残留数据
	}

	UpdateData(false);//将变量的内容同步到控件中
}
```

###### 清除数据按钮：

```c
void CUpperMachineDlg::OnBnClickedButton3()//清除数据
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_EditRxData.Empty();
	UpdateData(FALSE);//从控件同步到控制台显示
}
```

###### 保存数据按钮：

```c
void CUpperMachineDlg::OnBnClickedButton2()//保存数据
{
	// TODO: 在此添加控件通知处理程序代码
	CString FileName;
	//CFileDialog dlgFile(FALSE, _T("*.txt"), NULL, OFN_HIDEREADONLY, _T("Describe File(*.txt)|*.txt|All Files(*.*)|*.*||"), NULL);
	//if (IDOK == dlgFile.DoModal())
	//{
	//	FileName = dlgFile.GetPathName();
	//}
	//else
	//{
	//	return;
	//}
	FileName = "D:\\data.txt";
	CStdioFile FileWrite;
	//CFile::modeNoTruncate表示给文件追加 ，不覆盖原有内容
	if (!FileWrite.Open(FileName, CFile::modeWrite | CFile::modeCreate | CFile::typeText| CFile::modeNoTruncate, NULL))
	{
		AfxMessageBox(_T("打开文件失败!"));
		return;
	}
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString temp ,time;
	temp =m_EditRxData;
	time.Format(_T("[%d-%d-%d %d:%d:%d]\n"),t.wYear,t.wMonth,t.wDay, t.wHour, t.wMinute, t.wSecond);//格式化时间
	//去除多余换行符
 
	FileWrite.SeekToEnd();    //表示在文件最后添加。
	FileWrite.WriteString(time);
	FileWrite.WriteString(temp);
	FileWrite.WriteString(_T("\n"));
	FileWrite.Close();
	AfxMessageBox(_T("保存成功！"));
}
```

##### 4.2.4效果展示：

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/IMG_20220612_113502.jpg)

程序可正常打开串口，并接受信息，点击保存文件可以自动在D盘根目录生成data.txt文件。

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1655005112446.png)

数据保存效果：

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1655005191264.png)

### 五、测量干电池电压信息

由于手头没有合适的电阻，无法搭建外围分压电路，所以测量的量程只有0~3.3V.故用A0口来测量一节干电池的电压，拿A1口来测量两节干电池串联后的电压（不超过量程）。（电池负极接地，正极接A0和A1）

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/IMG_20220612_120719.jpg)

测试得到一节干电池电压大约为：1.62V  两节串联为：3.24V（碱性干电池）符合电池的理论电压值。

保存数据：

![](https://image-1308319148.cos.ap-chengdu.myqcloud.com/main/1655007504691.png)

### 六、整体反思与评价

本次作业是比较简单的一个单片机和上位机的通信历程，但也是我第一次从单片机开发到上位机开发走过一个完整的开发流程。串口通信，ADC转换的内容也是我第一次接触，参考网上大量开源教程，博客后才做出这个简单的系统，在此记录一下。