#include "sys.h"
#include "f3.h"
/**********		USB		************/
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "iap.h"

void IO_Init(void);
void IO_DeInit(void);

int main()
{
	IO_Init();
	if (((*(vu32 *)(*(vu32 *)0x0803FFFC)) & 0x2FFE0000) == 0x20000000 && PBin(12) == 1)
	{
		IO_DeInit(); 											//没按KEY1且APP栈顶地址合法就跳转到APP
		jump_app(*(vu32 *)0x0803FFFC);		//APP起始地址存放在0x0803FFFC
	}
	//如果按下了KEY1或APP栈顶地址不合法就进入BootLoader
	delay_init();
	MyUSB_Init();


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);		//EMABLE CRC CLK

	for (;;)
	{
		iap_loop();
	}
}

//初始化IO,LED,KEY
void IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStruct); //PB0:RF3_ON  PB1:RF2_ON  PB2:RF1_ON
	PBout(0) = 0;
	PBout(1) = 0;
	PBout(2) = 0;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStruct); //PC6:LED_B  PC7:LED_G  PC8:LED_R
	PCout(6) = 0;						//TIM8_CH1		TIM8_CH2		TIM8_CH3
	PCout(7) = 0;
	PCout(8) = 0;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct); //PB12:KEY1  PB13:KEY2  PB14:KEY3
}

void IO_DeInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
}
