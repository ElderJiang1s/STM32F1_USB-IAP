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


_usb_usart_fifo uu_txfifo;				   //USB���ڷ���FIFO�ṹ��
u8 USART_PRINTF_Buffer[USB_USART_REC_LEN]; //usb_printf���ͻ�����

//�����ƴ���1�������ݵķ���,������USB���⴮�ڽ��յ�������.
u8 USB_USART_RX_BUF[USB_USART_REC_LEN] = {0}; //���ջ���,���USART_REC_LEN���ֽ�.
u8 isRXstatus = 1;							  //���Ϊ1˵�����ڵȴ������µ����ݰ�
											  //���Ϊ0˵���Ѿ��յ��˳��Ȱ������ڽ�������

extern LINE_CODING linecoding; //USB���⴮��������Ϣ
/////////////////////////////////////////////////////////////////////////////////
//��USB����ͨ�ò��ִ���,ST����USB����,�˲��ִ��붼���Թ���.
//�˲��ִ���һ�㲻��Ҫ�޸�!

void MyUSB_Init(void)
{
	delay_ms(100);
	USB_Port_Set(0); //USB�ȶϿ�
	delay_ms(100);
	USB_Port_Set(1); //USB�ٴ�����
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
}

//USB�����жϷ�����
void USBWakeUp_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line18); //���USB�����жϹ���λ
}

//USB�жϴ�����
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}

//USBʱ�����ú���,USBclk=48Mhz@HCLK=72Mhz
void Set_USBClock(void)
{
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);	//USBclk=PLLclk/1.5=48Mhz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); //USBʱ��ʹ��
}

//USB����͹���ģʽ
//��USB����suspendģʽʱ,MCU����͹���ģʽ
//��������ӵ͹��Ĵ���(�����ʱ�ӵ�)
void Enter_LowPowerMode(void)
{
	printf("usb enter low power mode\r\n");
	bDeviceState = SUSPENDED;
}

//USB�˳��͹���ģʽ
//�û��������������ش���(������������ʱ�ӵ�)
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	printf("leave low power mode\r\n");
	if (pInfo->Current_Configuration != 0)
		bDeviceState = CONFIGURED;
	else
		bDeviceState = ATTACHED;
}

//USB�ж�����
void USB_Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
	//  ������18�ϵ��ж�
	EXTI_InitStructure.EXTI_Line = EXTI_Line18;			   // USB resume from suspend mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //line 18���¼��������ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the USB interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; //��2�����ȼ���֮
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn; //��2�����ȼ����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//USB�ӿ�����
void USB_Cable_Config(FunctionalState NewState)
{
	if (NewState != DISABLE)
		printf("usb pull up enable\r\n");
	else
		printf("usb pull up disable\r\n");
}

//USBʹ������/����
//enable:0,�Ͽ�
//       1,��������
void USB_Port_Set(u8 enable)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PORTAʱ��
	if (enable)
		_SetCNTR(_GetCNTR() & (~(1 << 1))); //�˳��ϵ�ģʽ
	else
	{
		_SetCNTR(_GetCNTR() | (1 << 1)); // �ϵ�ģʽ
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12) = 0;
	}
}

//��ȡSTM32��ΨһID
//����USB������Ϣ
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

//��32λ��ֵת����unicode.
//value,Ҫת����ֵ(32bit)
//pbuf:�洢��ַ
//len:Ҫת���ĳ���
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

//USB COM�ڵ�������Ϣ,ͨ���˺�����ӡ����.
bool USART_Config(void)
{
	uu_txfifo.readptr = 0;  //��ն�ָ��
	uu_txfifo.writeptr = 0; //���дָ��
	return (TRUE);
}

///�����USB���⴮�ڽ��յ�������
//databuffer:���ݻ�����
//Nb_bytes:���յ����ֽ�����С�ڵ���0x40
int RXLen = 0; 			//�Ѿ����յĳ���
int needRXLen = 0; 	//��Ҫ���յĳ��ȣ���λ�ֽ�
void usb_data_process(u8 *data_buffer, u8 Nb_bytes)
{
	if (isRXstatus == 1) //���ڽ��յ��İ���ǰ4�ֽ��ǳ���
	{
		RXLen = 0;
		if (((u32 *)data_buffer)[0] * 4 == Nb_bytes) //�����һ������û�д���0x40byte
		{
			memcpy(USB_USART_RX_BUF, data_buffer, Nb_bytes); //��������
			RXLen = Nb_bytes;								 //�Ѿ����յĳ��ȣ���λ4�ֽ�
			IAP_receiveData((u32 *)USB_USART_RX_BUF, RXLen);
			RXLen = 0;
		}
		else if (((u32 *)data_buffer)[0] * 4 > Nb_bytes) //�����һ�����ݴ���0x40byte����Ҫ��ν���
		{
			isRXstatus = 0;						//�����һ�ν��յ���0x40byte�Ĳ���
			needRXLen = ((u32 *)data_buffer)[0] * 4; //ȡ����Ҫ���յĳ��ȣ���λ�ֽ�
			memcpy(USB_USART_RX_BUF, data_buffer,Nb_bytes);
			RXLen = Nb_bytes; //����Nb_bytesӦ�õ���0x40
		}
		else
		{
			__asm("nop"); //��������ִ�е�����
		}
	}
	else //���ڽ��ճ���0x40byte�Ĳ���
	{
		memcpy(USB_USART_RX_BUF + RXLen, data_buffer, Nb_bytes);
		RXLen += Nb_bytes;		//����������һ������Nb_bytesӦ�õ���0x40
		if (RXLen >= needRXLen) //�Ѿ����������е�����
		{
			isRXstatus = 1; //�����һ�δ�ͷ��ʼ����
			IAP_receiveData((u32 *)USB_USART_RX_BUF, RXLen >> 2);
			RXLen = 0;
		}
	}
}

//����һ���ֽ����ݵ�USB���⴮��
void USB_USART_SendData(u8 data)
{
	uu_txfifo.buffer[uu_txfifo.writeptr] = data;
	uu_txfifo.writeptr++;
	if (uu_txfifo.writeptr == USB_USART_TXFIFO_SIZE) //����buf��С��,����.
	{
		uu_txfifo.writeptr = 0;
	}
}

//����num���ֽ����ݵ�USB���⴮��
//������ͳɹ�����0��buf�������1
u8 USB_USART_SendMulData(u8 *buffer, u32 num)
{
	memcpy(&uu_txfifo.buffer[uu_txfifo.writeptr], buffer, num);
	uu_txfifo.writeptr += num;
	if (uu_txfifo.writeptr >= USB_USART_TXFIFO_SIZE) //����buf��С��,����.
	{
		uu_txfifo.writeptr = 0;
		return 1;
	}
	return 0;
}

//usb���⴮��,printf ����
//ȷ��һ�η������ݲ���USB_USART_REC_LEN�ֽ�
void usb_printf(char *fmt, ...)
{
	u16 i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)USART_PRINTF_Buffer, fmt, ap);
	va_end(ap);
	i = strlen((const char *)USART_PRINTF_Buffer); //�˴η������ݵĳ���
	for (j = 0; j < i; j++)						   //ѭ����������
	{
		USB_USART_SendData(USART_PRINTF_Buffer[j]);
	}
}
