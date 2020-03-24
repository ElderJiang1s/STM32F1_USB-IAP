#ifndef __APP1_H
#define __APP1_H

#pragma once

#include "nmsl/nmsl.h"
#include <QtWidgets/QMainWindow>
#include "ui_app1.h"
#include "STM32_IAP.h"


class app1 : public QMainWindow
{
	Q_OBJECT

public:
	app1(QWidget *parent = Q_NULLPTR);
	~app1();

	QSerialPort *serialport;
	QTimer *tmr1;	//定时康康有没有串口
	void tmr1Slot();//定时康康有没有串口
	QList<QSerialPortInfo>  portlist;	//串口列表
	int portNum = 0;//当前系统中串口的数量
	QString indexPort;//当前选中的串口名
	QSerialPortInfo currentPort;	//当前选中的串口
	bool connectStatus = false;//当前连接状态
	QByteArray updateFile;//升级文件

	


	void cmbIndexChange(int index);//comboBox选中项发生改变
	void connectClicked();//连接按钮按下
	void openFile();//打开文件按钮按下
	void startUpdate();//开始升级按钮按下

	void sendData(QByteArray data);
	void readData();

	Ui::app1Class ui;
private:
	
	STM32_IAP* iap_app;
};






#endif
