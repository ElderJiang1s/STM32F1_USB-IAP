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

	//定时器1
	connect(tmr1, &QTimer::timeout, this, &app1::tmr1Slot);

	//comboBox选中项发生改变
	void (QComboBox:: * currentindec_change)(int) = &QComboBox::currentIndexChanged;
	connect(ui.comboBox, currentindec_change, this, &app1::cmbIndexChange);

	//连接按键按下
	connect(ui.btn_connect, &QPushButton::clicked, this, &app1::connectClicked);

	//打开文件按键按下
	connect(ui.btn_openfile, &QPushButton::clicked, this, &app1::openFile);

	//开始升级按键按下
	connect(ui.btn_startUpdate, &QPushButton::clicked, this, &app1::startUpdate);
}


//定时器slot，定时康康有没有串口连接或断开
void app1::tmr1Slot()
{
	portlist = QSerialPortInfo::availablePorts();								//获取系统中的串口
	if (portNum != portlist.length())											//如果串口数量变化了
	{
		indexPort = ui.comboBox->itemText(ui.comboBox->currentIndex());
		portNum = portlist.length();											//更新portNum
		ui.comboBox->clear();													//清空comboBox
		for (int i = 0; i < portlist.length(); i++)								//把所有的串口名放到comboBox里
		{
			ui.comboBox->addItem(QString(portlist[i].description() + "(" + portlist[i].portName() + ")"));
			if (indexPort.indexOf(portlist[i].portName()) != -1)
			{
				ui.comboBox->setCurrentIndex(i);
			}
		}
	}
}

//comboBox选中项发生改变
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
		//没有选中串口
	}
}

//打开文件按钮按下
void app1::openFile()
{
	ui.lineEdit->setText(QFileDialog::getOpenFileName(this, "打开文件", "C:\\Users\\Admin\\Desktop\\", "(*.bin)"));
	if (ui.lineEdit->text() != "")				//如果文件名不为空
	{
		QFile file;
		file.setFileName(ui.lineEdit->text());	//设置文件名
		file.open(QIODevice::ReadOnly);			//只读
		updateFile = file.readAll();			//读取文件
		ui.btn_startUpdate->setEnabled(true);	//开始升级按键可以按了
		ui.label_fileSize->setText(QString::number(updateFile.length()) + " Byte");	//标签显示文件长度
		ui.label_md5->setText(QCryptographicHash::hash(updateFile, QCryptographicHash::Md5).toHex());	//标签显示MD5
		iap_app->loadUpdataFile(updateFile);	//IAP软件加载文件
		file.close();							//关闭文件
	}

}

//连接按钮按下
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

		if (serialport->open(QIODevice::ReadWrite) == true)//如果成功连接
		{
			connectStatus = true;
			ui.btn_openfile->setEnabled(true);
			ui.btn_connect->setText("已连接");
			cout << "Connect" << currentPort.portName().toUtf8().data() << "Success";
			connect(serialport, &QSerialPort::readyRead, this, &app1::readData);
			iap_app->connect();//连接软件
		}
	}
	else
	{
		connectStatus = false;
		ui.btn_connect->setText("连接");
		ui.btn_openfile->setEnabled(false);
		cout << "Disconnect" << currentPort.portName().toUtf8().data() << "Success";
		serialport->close();
		delete serialport;
	}
}

//开始升级按钮按下
void app1::startUpdate()
{
	iap_app->startUpdata();
}


//发送数据
void app1::sendData(QByteArray data)
{
	if (connectStatus == true)//如果已连接
	{
		serialport->write(data);
	}
}

//接收到了数据
void app1::readData()
{
	iap_app->receiveData(serialport->readAll());
}