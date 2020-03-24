#ifndef __STM32_IAP_H
#define __STM32_IAP_H

#pragma once

#include <QObject>
#include "nmsl/nmsl.h"
#include "ui_app1.h"
//#include "app1.h"

extern class app1;
//extern app1 w;

class STM32_IAP : public QObject
{
public:

	STM32_IAP(app1*);

	void sendData(QByteArray data);
	bool connect(int t = 10);
	bool erase();
	bool Programming();
	void receiveData(QByteArray data);
	void loadUpdataFile(QByteArray file);
	void STM32_IAP::startUpdata();

	//发送GETINF帧
	bool send_GETINF(QVector<unsigned int>& data, int timeout);

	//发送UPDATE帧
	bool send_UPDATE(QByteArray data, uint t, int timeout);
	
	//发送FINISH帧
	bool send_FINISH(int timeout);

	//发送REBOOT帧
	bool send_REBOOT(int timeout);

	//发送SIZE帧
	bool send_SIZE(unsigned int size, int timeout);

	//发送BASEADDR帧
	bool send_BASEADDR(unsigned int addr, int timeout);

	//发送GETERASE帧
	int STM32_IAP::send_GETERASE(int timeout);


	//计算CRC32，兼容STM32
	unsigned int cal_crc(unsigned int* ptr, int len);
private:
	QByteArray updateFile;//升级文件
	app1* app;//app1.h
	bool isReceiveReady = false;//是否有数据需要处理
	QByteArray receiveBuffer;

	QString userID;
	QString BLversion;
	QString APPversion;


};



#endif

