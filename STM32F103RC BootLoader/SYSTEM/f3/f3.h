#ifndef __F3_H
#define __F3_H
#include "sys.h"





#define led1 PAout(0)
#define led2 PAout(1)


/********************	NVIC	********************/
void nvic_cfg(u8 IRQChannel,u8 Preemption,u8 Sub);

/********************	Circular *****************/
typedef struct
{
	u32 AllSize;
	u8* Start;
	u8* End;
	volatile u8* ReadPointer;
	volatile u8* WritePointer;
}Circular;
Circular* Circular_Init(u32 Size);
u8 Circular_Read(Circular* ss);
void Circular_Write(Circular* ss,u8 ToWrite);
void Circular_Free(Circular* ss);


/********************	GPIO	********************/
void led_init(void);//LED1:PA0	LED2:PA1
void key_init(void);//KEY Pin:PB5

/********************	TIMER	********************/
void TIM8_OUT_Init(void);
void TIM4_PWM_Init(void);
void TIM6_IT_Init(u16 ARR,u16 PSC);

/********************	ADC		********************/


/********************	DAC		********************/
void DAC1_OUT_Init(void);//PA4

/********************	DMA		********************/


/********************	RTC		********************/
void RTC_Init(void);

/********************	OTHER	********************/
float mapf(float val, float I_Min, float I_Max, float O_Min, float O_Max);
s32 map(s32 val, s32 I_Min, s32 I_Max, s32 O_Min, s32 O_Max);
void Sonar_Init(void);
double Sonar_GetDistance(void);

#endif
