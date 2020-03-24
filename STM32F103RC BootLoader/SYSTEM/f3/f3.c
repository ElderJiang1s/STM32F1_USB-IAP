#include "f3.h"
//2018年12月16日02:28:19		添加Circular
//2019年1月1日17:16:37			添加Sonar
//2019年2月8日00:07:32			添加DAC


/************************************************************/
/*            						NVIC				                      */
/************************************************************/
void nvic_cfg(u8 IRQChannel,u8 Preemption,u8 Sub)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=IRQChannel;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=Preemption;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=Sub;
	NVIC_Init(&NVIC_InitStruct);	
}

/************************************************************/
/*            						Circular			                    */
/************************************************************/

Circular* Circular_Init(u32 Size)
{
	Circular* tmp;
	tmp=(Circular*)mymalloc(20);
	tmp->AllSize=Size;
	tmp->AllSize=Size;
	tmp->Start=(u8*)mymalloc(tmp->AllSize);
	tmp->End=tmp->Start+tmp->AllSize;
	tmp->ReadPointer=(u8*)tmp->Start;
	tmp->WritePointer=(u8*)tmp->Start;
	return(tmp);
}

u8 Circular_Read(Circular* ss)
{
	u8 tmp;
	tmp=*ss->ReadPointer;
	if(ss->ReadPointer==ss->End)
	{
		ss->ReadPointer=ss->Start;
	}
	else
	{
		ss->ReadPointer=ss->ReadPointer+1;
	}
	return(tmp);
}

void Circular_Write(Circular* ss,u8 ToWrite)
{
	*ss->WritePointer=ToWrite;
	if(ss->WritePointer==ss->End)
	{
		ss->WritePointer=ss->Start;
	}
	else
	{
		ss->WritePointer=ss->WritePointer+1;
	}
}

void Circular_Free(Circular* ss)
{
	myfree(ss->Start);
	myfree(ss);
}


/************************************************************/
/*            						GPIO				                      */
/************************************************************/
void led_init(void)//LED1:PA0	LED2:PA1
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void key_init(void)//KEY Pin:PB5	EXTI9_5_IRQHandler()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);//F4:SYSCFG_EXTILineConfig
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	EXTI_InitStruct.EXTI_Line=EXTI_Line5;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);			
	nvic_cfg(EXTI9_5_IRQn,2,2);
}



/**********************************************************/
/*            						TIMER					                  */
/**********************************************************/
void TIM8_OUT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStruct);//CH1:PC6	CH2:PC7	CH3:PC8	CH4:PC9
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=20000;
	TIM_TimeBaseInitStruct.TIM_Prescaler=71;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=1000;//1000停转	2000满转
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);
	TIM_OC2Init(TIM8,&TIM_OCInitStruct);
	TIM_OC3Init(TIM8,&TIM_OCInitStruct);
	TIM_OC4Init(TIM8,&TIM_OCInitStruct);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	TIM_Cmd(TIM8,ENABLE);
}

void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	//PB8:TIM4_CH3
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=1<<8;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=0xFF;
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;
	TIM_OC3Init(TIM4,&TIM_OCInitStruct);
	TIM_Cmd(TIM4,ENABLE);
}

void TIM6_IT_Init(u16 ARR,u16 PSC)//TIM6_IRQHandler
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=ARR;
	TIM_TimeBaseInitStruct.TIM_Prescaler=PSC;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	nvic_cfg(TIM6_IRQn,1,2);
	TIM_Cmd(TIM6,ENABLE);
}

/**********************************************************/
/*            						ADC						                  */
/**********************************************************/


/**********************************************************/
/*            						DAC						                  */
/**********************************************************/
void DAC1_OUT_Init(void)//PA4
{
	DAC_InitTypeDef DAC_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	DAC_InitStruct.DAC_OutputBuffer=DAC_OutputBuffer_Disable;
	DAC_InitStruct.DAC_Trigger=DAC_Trigger_None;
	DAC_InitStruct.DAC_WaveGeneration=DAC_WaveGeneration_None;
	DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
	DAC_Init(DAC_Channel_1,&DAC_InitStruct);
	DAC_Cmd(DAC_Channel_1,ENABLE);
}

/**********************************************************/
/*            						DMA						                  */
/**********************************************************/



/**********************************************************/
/*								     RTC                                */
/**********************************************************/
void RTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);			//使能PWR时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);			//使能BKP时钟
	RTC_EnterConfigMode();								//允许RTC配置									RTC->CRL  CNF
	RTC_WaitForLastTask();									//等待上面操作完成							RTC->CRL  RTOFF
	PWR_BackupAccessCmd(ENABLE);								//使能后备寄存器访问
	BKP_DeInit();												//重置后备寄存器
	RCC_LSEConfig(RCC_LSE_ON);									//打开LSE晶振 32.768KHz
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);			//等待LSE稳定
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//RTC时钟源LSE							RTC->BDCR
	RTC_WaitForLastTask();									//等待上面操作完成							RTC->CRL  RTOFF
	RCC_RTCCLKCmd(ENABLE);										//使能RTC时钟
	RTC_SetPrescaler(32767);									//RTC分频系数							RTC->PRL
	RTC_WaitForLastTask();									//等待上面操作完成							RTC->CRL  RTOFF
	//RTC_SetCounter();											//设置当前计数值							RTC->CNT
	RTC_ITConfig(RTC_IT_SEC,ENABLE);							//设置秒中断
	nvic_cfg(RTC_IRQn,0,0);
	RTC_ExitConfigMode();								//退出RTC配置									RTC->CRL  CNF
	RTC_WaitForLastTask();									//等待上面操作完成							RTC->CRL  RTOFF
}

/************************************************************/
/*            						OTHER				                      */
/************************************************************/
float mapf(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	return(val/(I_Max-I_Min)*(O_Max-O_Min) + O_Min);
}

s32 map(s32 val, s32 I_Min, s32 I_Max, s32 O_Min, s32 O_Max)
{
	return(val/(I_Max-I_Min)*(O_Max-O_Min) + O_Min);
}

void Sonar_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//PB8:Trig	PB9:Echo
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	PBout(8)=0;
	PBout(9)=0;
}

double Sonar_GetDistance(void)
{
	u32 nus;
	nus=0;
	PBout(8)=1;
	delay_us(20);
	PBout(8)=0;
	while(PBin(9)==0);
	while(PBin(9)==1){nus++;}
	return(((double)nus)*0.017);
}

