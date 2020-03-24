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
	QTimer *tmr1;	//��ʱ������û�д���
	void tmr1Slot();//��ʱ������û�д���
	QList<QSerialPortInfo>  portlist;	//�����б�
	int portNum = 0;//��ǰϵͳ�д��ڵ�����
	QString indexPort;//��ǰѡ�еĴ�����
	QSerialPortInfo currentPort;	//��ǰѡ�еĴ���
	bool connectStatus = false;//��ǰ����״̬
	QByteArray updateFile;//�����ļ�

	


	void cmbIndexChange(int index);//comboBoxѡ������ı�
	void connectClicked();//���Ӱ�ť����
	void openFile();//���ļ���ť����
	void startUpdate();//��ʼ������ť����

	void sendData(QByteArray data);
	void readData();

	Ui::app1Class ui;
private:
	
	STM32_IAP* iap_app;
};






#endif
