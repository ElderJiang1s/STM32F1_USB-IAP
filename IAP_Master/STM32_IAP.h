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

	//����GETINF֡
	bool send_GETINF(QVector<unsigned int>& data, int timeout);

	//����UPDATE֡
	bool send_UPDATE(QByteArray data, uint t, int timeout);
	
	//����FINISH֡
	bool send_FINISH(int timeout);

	//����REBOOT֡
	bool send_REBOOT(int timeout);

	//����SIZE֡
	bool send_SIZE(unsigned int size, int timeout);

	//����BASEADDR֡
	bool send_BASEADDR(unsigned int addr, int timeout);

	//����GETERASE֡
	int STM32_IAP::send_GETERASE(int timeout);


	//����CRC32������STM32
	unsigned int cal_crc(unsigned int* ptr, int len);
private:
	QByteArray updateFile;//�����ļ�
	app1* app;//app1.h
	bool isReceiveReady = false;//�Ƿ���������Ҫ����
	QByteArray receiveBuffer;

	QString userID;
	QString BLversion;
	QString APPversion;


};



#endif

