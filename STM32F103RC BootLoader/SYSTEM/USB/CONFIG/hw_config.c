#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usart.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
//#include "includes.h"
#include "iap.h"


_usb_usart_fifo uu_txfifo;				   //USB串口发送FIFO结构体
u8 USART_PRINTF_Buffer[USB_USART_REC_LEN]; //usb_printf发送缓冲区

//用类似串口1接收数据的方法,来处理USB虚拟串口接收到的数据.
u8 USB_USART_RX_BUF[USB_USART_REC_LEN] = {0}; //接收缓冲,最大USART_REC_LEN个字节.
u8 isRXstatus = 1;							  //如果为1说明正在等待接收新的数据包
											  //如果为0说明已经收到了长度包，正在接收其他

extern LINE_CODING linecoding; //USB虚拟串口配置信息
/////////////////////////////////////////////////////////////////////////////////
//各USB例程通用部分代码,ST各各USB例程,此部分代码都可以共用.
//此部分代码一般不需要修改!

void MyUSB_Init(void)
{
	delay_ms(100);
	USB_Port_Set(0); //USB先断开
	delay_ms(100);
	USB_Port_Set(1); //USB再次连接
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
}

//USB唤醒中断服务函数
void USBWakeUp_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line18); //清除USB唤醒中断挂起位
}

//USB中断处理函数
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}

//USB时钟配置函数,USBclk=48Mhz@HCLK=72Mhz
void Set_USBClock(void)
{
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);	//USBclk=PLLclk/1.5=48Mhz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); //USB时钟使能
}

//USB进入低功耗模式
//当USB进入suspend模式时,MCU进入低功耗模式
//需自行添加低功耗代码(比如关时钟等)
void Enter_LowPowerMode(void)
{
	printf("usb enter low power mode\r\n");
	bDeviceState = SUSPENDED;
}

//USB退出低功耗模式
//用户可以自行添加相关代码(比如重新配置时钟等)
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	printf("leave low power mode\r\n");
	if (pInfo->Current_Configuration != 0)
		bDeviceState = CONFIGURED;
	else
		bDeviceState = ATTACHED;
}

//USB中断配置
void USB_Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
	//  开启线18上的中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line18;			   // USB resume from suspend mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //line 18上事件上升降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the USB interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; //组2，优先级次之
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn; //组2，优先级最高
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//USB接口配置
void USB_Cable_Config(FunctionalState NewState)
{
	if (NewState != DISABLE)
		printf("usb pull up enable\r\n");
	else
		printf("usb pull up disable\r\n");
}

//USB使能连接/断线
//enable:0,断开
//       1,允许连接
void USB_Port_Set(u8 enable)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PORTA时钟
	if (enable)
		_SetCNTR(_GetCNTR() & (~(1 << 1))); //退出断电模式
	else
	{
		_SetCNTR(_GetCNTR() | (1 << 1)); // 断电模式
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12) = 0;
	}
}

//获取STM32的唯一ID
//用于USB配置信息
void Get_SerialNum(void)
{
	u32 Device_Serial0, Device_Serial1, Device_Serial2;
	Device_Serial0 = *(u32 *)(0x1FFFF7E8);
	Device_Serial1 = *(u32 *)(0x1FFFF7EC);
	Device_Serial2 = *(u32 *)(0x1FFFF7F0);
	Device_Serial0 += Device_Serial2;
	if (Device_Serial0 != 0)
	{
		IntToUnicode(Device_Serial0, &Virtual_Com_Port_StringSerial[2], 8);
		IntToUnicode(Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
	}
}

//将32位的值转换成unicode.
//value,要转换的值(32bit)
//pbuf:存储地址
//len:要转换的长度
void IntToUnicode(u32 value, u8 *pbuf, u8 len)
{
	u8 idx = 0;
	for (idx = 0; idx < len; idx++)
	{
		if (((value >> 28)) < 0xA)
		{
			pbuf[2 * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}
		value = value << 4;
		pbuf[2 * idx + 1] = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////

//USB COM口的配置信息,通过此函数打印出来.
bool USART_Config(void)
{
	uu_txfifo.readptr = 0;  //清空读指针
	uu_txfifo.writeptr = 0; //清空写指针
	return (TRUE);
}

///处理从USB虚拟串口接收到的数据
//databuffer:数据缓存区
//Nb_bytes:接收到的字节数，小于等于0x40
int RXLen = 0; 			//已经接收的长度
int needRXLen = 0; 	//需要接收的长度，单位字节
void usb_data_process(u8 *data_buffer, u8 Nb_bytes)
{
	if (isRXstatus == 1) //现在接收到的包的前4字节是长度
	{
		RXLen = 0;
		if (((u32 *)data_buffer)[0] * 4 == Nb_bytes) //如果这一包数据没有大于0x40byte
		{
			memcpy(USB_USART_RX_BUF, data_buffer, Nb_bytes); //接收数据
			RXLen = Nb_bytes;								 //已经接收的长度，单位4字节
			IAP_receiveData((u32 *)USB_USART_RX_BUF, RXLen);
			RXLen = 0;
		}
		else if (((u32 *)data_buffer)[0] * 4 > Nb_bytes) //如果这一包数据大于0x40byte，需要多次接收
		{
			isRXstatus = 0;						//标记下一次接收的是0x40byte的部分
			needRXLen = ((u32 *)data_buffer)[0] * 4; //取出需要接收的长度，单位字节
			memcpy(USB_USART_RX_BUF, data_buffer,Nb_bytes);
			RXLen = Nb_bytes; //这里Nb_bytes应该等于0x40
		}
		else
		{
			__asm("nop"); //正常不会执行到这里
		}
	}
	else //现在接收超过0x40byte的部分
	{
		memcpy(USB_USART_RX_BUF + RXLen, data_buffer, Nb_bytes);
		RXLen += Nb_bytes;		//这里除了最后一包数据Nb_bytes应该等于0x40
		if (RXLen >= needRXLen) //已经接收了所有的数据
		{
			isRXstatus = 1; //标记下一次从头开始接收
			IAP_receiveData((u32 *)USB_USART_RX_BUF, RXLen >> 2);
			RXLen = 0;
		}
	}
}

//发送一个字节数据到USB虚拟串口
void USB_USART_SendData(u8 data)
{
	uu_txfifo.buffer[uu_txfifo.writeptr] = data;
	uu_txfifo.writeptr++;
	if (uu_txfifo.writeptr == USB_USART_TXFIFO_SIZE) //超过buf大小了,归零.
	{
		uu_txfifo.writeptr = 0;
	}
}

//发送num个字节数据到USB虚拟串口
//如果发送成功返回0，buf溢出返回1
u8 USB_USART_SendMulData(u8 *buffer, u32 num)
{
	memcpy(&uu_txfifo.buffer[uu_txfifo.writeptr], buffer, num);
	uu_txfifo.writeptr += num;
	if (uu_txfifo.writeptr >= USB_USART_TXFIFO_SIZE) //超过buf大小了,归零.
	{
		uu_txfifo.writeptr = 0;
		return 1;
	}
	return 0;
}

//usb虚拟串口,printf 函数
//确保一次发送数据不超USB_USART_REC_LEN字节
void usb_printf(char *fmt, ...)
{
	u16 i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)USART_PRINTF_Buffer, fmt, ap);
	va_end(ap);
	i = strlen((const char *)USART_PRINTF_Buffer); //此次发送数据的长度
	for (j = 0; j < i; j++)						   //循环发送数据
	{
		USB_USART_SendData(USART_PRINTF_Buffer[j]);
	}
}
