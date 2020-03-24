#include "iap.h"

s32 eraseVal = 0; 	//擦除进度
u32 fileSize = 0; 	//文件尺寸
u32 baseAddr = 0; 	//文件写入的地址偏移
u32 write_addr;   	//升级时文件写入的地址
u8 needErase = 0;		//是否需要擦除
u8 needReboot = 0;	//是否需要重启


//计算CRC32
//buffer：数据
//len：数据的长度，单位4字节
static u32 cal_crc(u32 *buffer, u32 len)
{
	int i;
	CRC->CR = 0x1;
	for (i = 0; i < len; i++)
	{
		CRC->DR = buffer[i];
	}
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	return CRC->DR;
}

//收到数据后调用
//buffer：接收到的数据
//len：数据的长度，单位4字节
void IAP_receiveData(u32 *buffer, u32 len)
{
	//一次最多接收64byte数据，要在EP3_OUT_Callback中先进行数据拼接
	if (cal_crc(buffer, buffer[0] - 1) == buffer[buffer[0] - 1]) //如果CRC32正确
	{
		switch (buffer[1])
		{
		case 0x01: 					//功能字：GETINF
			IAP_sendINF();		//返回INF包
			break;

		case 0x02: 					//功能字：UPDATE
			IAP_Update(&buffer[3], buffer[0] - 4);									//升级
			IAP_sendACK(0x02, buffer[buffer[0] - 1], buffer[2]); 		//返回ACK
			break;

		case 0x03:					//功能字：FINISH
			IAP_sendACK(0x03, buffer[2], 0);	//返回ACK		
			break;

		case 0x04: 					//功能字：REBOOT
			IAP_sendACK(0x04, buffer[2], 0);	//返回ACK
			needReboot = 1;										//主loop要进行重启，如果在这里重启ACK就发不出去
			break;

		case 0x05:					//功能字：SIZE
			fileSize = buffer[2];			 				//升级文件大小
			IAP_sendACK(0x05, buffer[3], 0); 	//返回ACK
			break;

		case 0x06:				  			 					//功能字：BASEADDR
			baseAddr = buffer[2];  						//文件写入的地址偏移
			baseAddr += 0x8000000; 						//地址偏移加上基地址
			write_addr = baseAddr;						//写入FLASH的地址
			write_baseAddr(baseAddr);		 			//写入到FLASH
			IAP_sendACK(0x06, buffer[3], 0); 	//返回ACK
			eraseVal = 0;					 						//擦除进度清0
			needErase = 1;					 					//等下要进行擦除
			break;

		case 0x07:				 									//功能字：GETERASE
			IAP_sendERASE(eraseVal);					//返回擦除进度
			break;

		}
	}
}

//CDC发送数据，内部调用
//buffer：数据
//len：数据的长度，单位4字节
void IAP_sendData(u32 *buffer, u32 len)
{
	USB_USART_SendMulData((u8 *)(buffer), len << 2);
}

//发送INF包
void IAP_sendINF(void)
{
	u32 tmp[8] = {0};

	tmp[0] = 0x08;				 					//len
	tmp[1] = 0x02;				 					//fun:INF
	tmp[2] = *(u32 *)0x1FFFF7E8; 		//STM32 ID1
	tmp[3] = *(u32 *)0x1FFFF7EC; 		//STM32 ID2
	tmp[4] = *(u32 *)0x1FFFF7F0; 		//STM32 ID3
	tmp[5] = 123;				 						//BL version
	tmp[6] = 456;				 						//APP version
	tmp[7] = cal_crc(tmp, 7);				//CRC32

	IAP_sendData(tmp, 8); 					//发送
}

//收到主机发来的包后发送ACK到主机
//fun：收到的包的fun
//crc32：收到包的crc32
void IAP_sendACK(u32 fun, u32 crc32, u32 res)
{
	u32 tmp[6] = {0};
	tmp[0] = 0x06;			  		//len
	tmp[1] = 0x01;			  		//fun
	tmp[2] = fun;			  			//receive fun
	tmp[3] = crc32;			  		//receive CRC32
	tmp[4] = res;			  			//保留
	tmp[5] = cal_crc(tmp, 5); //CRC32
	IAP_sendData(tmp, 6);	 		//发送
}

//收到GETERASE后发送ERASE包给主机
//val：擦除进度（0~100）
void IAP_sendERASE(s32 val)
{
	u32 tmp[4] = {0};
	tmp[0] = 0x03;						//len
	tmp[1] = 0x04;						//fun
	tmp[2] = val;							//擦除进度
	tmp[3] = cal_crc(tmp, 3);	//CRC32
	IAP_sendData(tmp, 4); 		//发送
}

//主loop循环调用
void iap_loop()
{
	if (needErase == 1)				//如果需要擦除
	{
		IAP_flashErase();				//擦除FLASH，同时更新进度变量eraseVal
		//在主loop中进行擦除，如果在ISR中进行擦除会无法响应主机的GETERASE包
		needErase = 0;					//擦除完成了
	}
	if(needReboot == 1)				//如果需要重启
	{
		delay_ms(100);					//延时100ms，确保ACK发送成功
		NVIC_SystemReset();			//重启
	}
}

//对flash进行擦除
//通过APP基地址baseAddr和文件长度fileSize计算需要擦除的页
//STM32F103RC FLASH长度256KB，每Page2KB，共有127页
void IAP_flashErase()
{
	u32 start_addr = baseAddr & 0xFFFFF800ul;					 						//要擦除的起始页地址，2KB对齐
	u32 end_addr = (baseAddr + fileSize + 0x800) & 0xFFFFF800ul; 	//要擦除的结束页地址，2KB对齐
	u32 pageNum = (end_addr - start_addr) / 0x800 + 1;			 			//要擦除的页数
	u32 i = 0;										//已经擦除的页数
	u32 j = 0;
	FLASH_Unlock();
	for (i = 0; i < pageNum; i++) //擦除每一页
	{
		for (j = 0; j < 0x400; j++) //查看这一页是否需要擦除
		{
			if (((u16 *)(i * 0x800 + start_addr))[i] != 0xFFFF)
				break; 			//如果全是0xFFFF就不擦除
		}
		if (j < 0x400) 	//如果这一页需要擦除
		{
			FLASH_ErasePage(i * 0x800 + start_addr); 		//擦除对应页
		}
		eraseVal = (s32)(((float)i / pageNum) * 100); //更新擦除进度
	}
	eraseVal = (s32)(((float)i / pageNum) * 100); 	//擦除进度这里等于100
	FLASH_Lock();
}

//升级
//data：存放升级的数据
//len：升级数据长度,单位4字节
void IAP_Update(u32 *data, u32 len)
{
	int i = 0;
	FLASH_Unlock();										//FLASH解锁
	for (i = 0; i < len; i++)					//把data中数据写入FLASH
		FLASH_ProgramWord(write_addr + (i << 2), data[i]);		//每次写入4字节
	FLASH_Lock();											//FLASH上锁
	write_addr += len << 2;						//更新写指针*/
}


//把APP起始地址写到FLASH末尾0x0803FFFC
//addr：APP起始地址
void write_baseAddr(u32 addr) 
{
	if (*(vu32 *)0x0803FFFC != addr)	 			//如果已有的不等于需要写入的
	{
		FLASH_Unlock();												//FLASH解锁
		FLASH_ErasePage(0x0803F800);		 			//擦除第127页
		FLASH_ProgramWord(0x0803FFFC, addr); 	//写入addr到0x0803FFFC
		FLASH_Lock();													//FLASH上锁
	}
}



typedef void (*iapfun)(void);
iapfun app;
//跳转到APP
//addr：APP起始地址
void jump_app(u32 addr)
{
	app = (iapfun) * (vu32 *)(addr + 4);	//APP第二个字是Reset_Handler
	MSR_MSP(*(vu32 *)addr);								//APP第一个字是MSP初始值
	app();																//跳转到APP
}

