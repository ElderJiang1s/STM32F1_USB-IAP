#ifndef __IAP_H
#define __IAP_H

#include "sys.h"
#include "f3.h"
#include "hw_config.h"


void IAP_receiveData(u32 *buffer, u32 len);						//收到数据后调用
void IAP_sendData(u32 *buffer, u32 len);							//CDC发送数据，内部调用
void IAP_sendINF(void);                               //发送INF包
void IAP_sendACK(u32 fun, u32 crc32, u32 res);				//收到主机发来的包后发送ACK到主机
void IAP_sendERASE(s32 val);													//收到GETERASE后发送ERASE包给主机
void iap_loop(void);																	//主loop循环调用
void IAP_flashErase(void);                            //对flash进行擦除
void IAP_Update(u32 *data, u32 len);                  //升级
void write_baseAddr(u32 addr);                 	      //把APP起始地址写到FLASH末尾0x0803FFFC
void jump_app(u32 addr);                            	//跳转到APP


#endif
