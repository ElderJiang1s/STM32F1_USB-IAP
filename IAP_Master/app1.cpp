#include "app1.h"

app1::~app1()
{
	delete iap_app;
}

app1::app1(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	tmr1 = new QTimer(this);
	tmr1->start(1000);

	

	iap_app = new STM32_IAP(this);//, &app1::sendData);

	ui.btn_openfile->setEnabled(false);
	ui.btn_startUpdate->setEnabled(false);

	//��ʱ��1
	connect(tmr1, &QTimer::timeout, this, &app1::tmr1Slot);

	//comboBoxѡ������ı�
	void (QComboBox:: * currentindec_change)(int) = &QComboBox::currentIndexChanged;
	connect(ui.comboBox, currentindec_change, this, &app1::cmbIndexChange);

	//���Ӱ�������
	connect(ui.btn_connect, &QPushButton::clicked, this, &app1::connectClicked);

	//���ļ���������
	connect(ui.btn_openfile, &QPushButton::clicked, this, &app1::openFile);

	//��ʼ������������
	connect(ui.btn_startUpdate, &QPushButton::clicked, this, &app1::startUpdate);
}


//��ʱ��slot����ʱ������û�д������ӻ�Ͽ�
void app1::tmr1Slot()
{
	portlist = QSerialPortInfo::availablePorts();								//��ȡϵͳ�еĴ���
	if (portNum != portlist.length())											//������������仯��
	{
		indexPort = ui.comboBox->itemText(ui.comboBox->currentIndex());
		portNum = portlist.length();											//����portNum
		ui.comboBox->clear();													//���comboBox
		for (int i = 0; i < portlist.length(); i++)								//�����еĴ������ŵ�comboBox��
		{
			ui.comboBox->addItem(QString(portlist[i].description() + "(" + portlist[i].portName() + ")"));
			if (indexPort.indexOf(portlist[i].portName()) != -1)
			{
				ui.comboBox->setCurrentIndex(i);
			}
		}
	}
}

//comboBoxѡ������ı�
void app1::cmbIndexChange(int index)
{
	if (index != -1)
	{
		cout << "index = " << index;
		cout << portlist[index].portName();
		currentPort = portlist[index];
	}
	else
	{
		//û��ѡ�д���
	}
}

//���ļ���ť����
void app1::openFile()
{
	ui.lineEdit->setText(QFileDialog::getOpenFileName(this, "���ļ�", "C:\\Users\\Admin\\Desktop\\", "(*.bin)"));
	if (ui.lineEdit->text() != "")				//����ļ�����Ϊ��
	{
		QFile file;
		file.setFileName(ui.lineEdit->text());	//�����ļ���
		file.open(QIODevice::ReadOnly);			//ֻ��
		updateFile = file.readAll();			//��ȡ�ļ�
		ui.btn_startUpdate->setEnabled(true);	//��ʼ�����������԰���
		ui.label_fileSize->setText(QString::number(updateFile.length()) + " Byte");	//��ǩ��ʾ�ļ�����
		ui.label_md5->setText(QCryptographicHash::hash(updateFile, QCryptographicHash::Md5).toHex());	//��ǩ��ʾMD5
		iap_app->loadUpdataFile(updateFile);	//IAP��������ļ�
		file.close();							//�ر��ļ�
	}

}

//���Ӱ�ť����
void app1::connectClicked()
{
	if (connectStatus == false)
	{
		serialport = new QSerialPort(currentPort);
		serialport->setBaudRate(115200);
		serialport->setDataBits(QSerialPort::Data8);
		serialport->setParity(QSerialPort::NoParity);
		serialport->setStopBits(QSerialPort::OneStop);
		serialport->setFlowControl(QSerialPort::NoFlowControl);

		if (serialport->open(QIODevice::ReadWrite) == true)//����ɹ�����
		{
			connectStatus = true;
			ui.btn_openfile->setEnabled(true);
			ui.btn_connect->setText("������");
			cout << "Connect" << currentPort.portName().toUtf8().data() << "Success";
			connect(serialport, &QSerialPort::readyRead, this, &app1::readData);
			iap_app->connect();//�������
		}
	}
	else
	{
		connectStatus = false;
		ui.btn_connect->setText("����");
		ui.btn_openfile->setEnabled(false);
		cout << "Disconnect" << currentPort.portName().toUtf8().data() << "Success";
		serialport->close();
		delete serialport;
	}
}

//��ʼ������ť����
void app1::startUpdate()
{
	iap_app->startUpdata();
}


//��������
void app1::sendData(QByteArray data)
{
	if (connectStatus == true)//���������
	{
		serialport->write(data);
	}
}

//���յ�������
void app1::readData()
{
	iap_app->receiveData(serialport->readAll());
}