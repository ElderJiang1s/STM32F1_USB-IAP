//#include "STM32_IAP.h"
#include "app1.h"

//p是app1.cpp中app1实例的指针
//方便操作窗口控件
//也能调用app1类里的函数
STM32_IAP::STM32_IAP(app1* p)
{
	app = p;
}


//串口接收到了数据，一次最多接收64字节，大于64字节的数据要多次接收
void STM32_IAP::receiveData(QByteArray data)
{
	receiveBuffer = data;
	isReceiveReady = true;
}

//串口发送数据
void STM32_IAP::sendData(QByteArray data)
{
	app->sendData(data);
}

//连接STM32，获取硬件ID和软件版本
//t:重试次数
bool STM32_IAP::connect(int t)
{
	QVector<unsigned int> data;
	app->ui.progressBar->setValue(0);
	while (t--)
	{
		if (send_GETINF(data, 100))
		{
			userID = QString("%1").arg(data[2], 8, 16, QLatin1Char('0'));//uint32转string，前面补0
			userID += QString("%1").arg(data[1], 8, 16, QLatin1Char('0'));
			userID += QString("%1").arg(data[0], 8, 16, QLatin1Char('0'));
			BLversion = QString("%1").arg(data[3]);
			APPversion = QString("%1").arg(data[4]);
			app->ui.label_3->setText("设备已连接 ID:");
			app->ui.label_stm32ID->setText(userID);
			app->ui.label_blVersion->setText(BLversion);
			app->ui.label_appVersion->setText(APPversion);
			return true;
		}
	}
	return false;
}


//载入升级文件
void STM32_IAP::loadUpdataFile(QByteArray file)
{
	updateFile = file;
	if (updateFile.size() % 4)//如果升级文件不是4的整数倍
	{
		updateFile += QByteArray(4 - (updateFile.size() % 4), 0);//文件末尾添加0
	}
}


//擦除STM32，并在进度条中显示进度
bool STM32_IAP::erase()
{
	int error_count = 0;
	int val = 0;
	if (send_SIZE(updateFile.size(), 100) == false)	//发送升级文件大小
	{
		cout << "send_SIZE FAIL";
		return false;
	}
	cout << "send_SIZE OK";

	if (send_BASEADDR(0xF000, 100) == false)//发送起始地址0x800F000，STM32返回ACK后开始擦除 boootloader 60KB
	{
		cout << "send_BASEADDR FAIL";	//BASEADDR发送失败
		return false;
	}
	cout << "send_BASEADDR OK";			//BASEADDR发送成功

	app->ui.btn_startUpdate->setText("正在擦除...");
	app->ui.progressBar->setValue(0);	//进度条清0
	while (true)
	{
		val = send_GETERASE(100);
		if (val != -1)
		{
			app->ui.progressBar->setValue(val);
		}
		else
		{
			error_count++;
			cout << "send_GETERASE FAIL";
		}
		if (error_count > 100 || val == 100)//如果错误超过100或擦除完成了
		{
			break;
		}
	}
	if (error_count > 100)	//如果错误超过100
	{
		app->ui.btn_startUpdate->setText("擦除失败");
		cout << "send_ERASE FAIL";
		return false;
	}
	if (val == 100)		//如果擦除成功
	{
		app->ui.btn_startUpdate->setText("擦除成功");
		cout << "send_ERASE OK";
	}
	return true;
}


bool STM32_IAP::Programming()
{
	int i;
	app->ui.btn_startUpdate->setText("正在升级...");
	app->ui.progressBar->setValue(0);	//进度条清0

	for (i = 0; i < updateFile.length() / 1024; i++)				//发送升级文件，每一包有1024byte数据
	{
		if (send_UPDATE(updateFile.mid(i * 1024, 1024), i, 500))
		{
			cout << "send_UPDATE" << i << "OK";
			app->ui.progressBar->setValue((int)((float)i / (updateFile.length() / 1024 + 1) * 100));
		}
		else
		{
			cout << "send_UPDATE" << i << "FAIL";
			app->ui.btn_startUpdate->setText("升级失败");
			return false;
		}
	}
	if (updateFile.length() % 1024)									//发送最后一包不到1024byte的数据
	{
		if (send_UPDATE(updateFile.mid(i * 1024), i, 500))
		{
			cout << "send_UPDATE" << i << "OK";
			app->ui.progressBar->setValue(100);

		}
		else
		{
			cout << "send_UPDATE" << i << "FAIL";
			app->ui.btn_startUpdate->setText("升级失败");
			return false;
		}
	}
	app->ui.btn_startUpdate->setText("升级成功");
	return true;
}


//点击了开始升级按钮
void STM32_IAP::startUpdata()
{
	clock_t t;
	if (erase() == false)
		return;
	t = clock();
	if (Programming() == false)
		return;
	t = clock() - t;
	if (send_FINISH(500))
	{
		cout << "send_FINISH" << "OK";
	}
	else
	{
		cout << "send_FINISH" << "FAIL";
		return;
	}
	if (send_REBOOT(200))
	{
		cout << "send_REBOOT" << "OK";
	}
	else
	{
		cout << "send_REBOOT" << "FAIL";
		return;
	}
	QMessageBox::information(app, "升级成功", QString("软件升级成功\r\n速度：%1 KB/S").arg((float)updateFile.size()/t));
}

//发送GETINF帧
//data[0:2]：STM32序列号
//data[3]：BL版本
//data[4]：APP版本
//timout：超时时间，单位ms
bool STM32_IAP::send_GETINF(QVector<unsigned int>& data, int timeout)
{
	int count = 0;										//超时计数
	unsigned int tmp[3] = { 0 };						//要发送的数据
	tmp[0] = 0x03;										//长度
	tmp[1] = 0x01;										//功能字
	tmp[2] = cal_crc(tmp, 2);							//计算CRC32
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//发送GETINF帧
	data.clear();										//清空data，等下要装数据
	while (1)//等待返回INF帧
	{	//如果收到了数据
		if (isReceiveReady)
		{	//收到的数据是INF
			if (((uint*)receiveBuffer.data())[1] == 0x02)
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 7) == ((uint*)receiveBuffer.data())[7])
				{
					data << ((uint*)receiveBuffer.data())[2];//STM32序列号1
					data << ((uint*)receiveBuffer.data())[3];//STM32序列号2
					data << ((uint*)receiveBuffer.data())[4];//STM32序列号3
					data << ((uint*)receiveBuffer.data())[5];//BL版本
					data << ((uint*)receiveBuffer.data())[6];//APP版本
					isReceiveReady = false;//标记数据已处理
					return true;
				}
			}
			isReceiveReady = false;			//如果CRC32不正确也标记数据已处理，等待数据再次到来
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//每20ms重新发送一次GETINF帧
	}
	return false;
}

//发送UPDATE帧
//data：要发送的文件数据，最多1024byte
//timout：超时时间，单位ms
bool STM32_IAP::send_UPDATE(QByteArray data, uint t, int timeout)
{
	int count = 0;
	unsigned int tmp[259] = { 0 };
	tmp[0] = 4 + (data.length() >> 2);							//长度
	tmp[1] = 0x02;												//功能字
	tmp[2] = t;													//数据包的序号
	for (int i = 0; i < (data.length() >> 2); i++)				//数据
		tmp[3 + i] = ((uint*)data.data())[i];					///*  + 优先级比 >> 高  */
	tmp[3 + (data.length() >> 2)] = cal_crc(tmp, 3 + (data.length() >> 2));	//CRC32
	sendData(QByteArray((char*)tmp, (4 + (data.length() >> 2)) * sizeof(int)));	//发送数据
	while (1)//等待返回ACK
	{	//如果收到了数据
		if (isReceiveReady)
		{	//功能字=ACK(0X01)且ACK的数据正确
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&							//收到包的功能字是ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&						//ACK的数据1是发送包的功能字0x02
				((uint*)receiveBuffer.data())[3] == tmp[3 + (data.length() >> 2)] &&	//ACK的数据2是发送包的CRC32
				((uint*)receiveBuffer.data())[4] == tmp[2])							//ACK的数据3是发送包序号
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		//if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//每20ms重新发送一次SIZE帧
	}
	return true;
}

//发送FINISH帧
//timout：超时时间，单位ms
bool STM32_IAP::send_FINISH(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x03;
	tmp[2] = cal_crc(tmp, 2);
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//发送FINISH帧
	while (1)//等待返回ACK
	{	//如果收到了数据
		if (isReceiveReady)
		{	//功能字=ACK(0X01)且ACK的数据正确
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//收到包的功能字是ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK的数据1是发送包的功能字0x04
				((uint*)receiveBuffer.data())[3] == tmp[2])		//ACK的数据2是发送包的CRC32
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//每20ms重新发送一次FINISH帧
	}
	return false;
}

//发送REBOOT帧
//timout：超时时间，单位ms
bool STM32_IAP::send_REBOOT(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x04;
	tmp[2] = cal_crc(tmp, 2);
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//发送REBOOT帧
	while (1)//等待返回ACK
	{	//如果收到了数据
		if (isReceiveReady)
		{	//功能字=ACK(0X01)且ACK的数据正确
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//收到包的功能字是ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK的数据1是发送包的功能字0x04
				((uint*)receiveBuffer.data())[3] == tmp[2])		//ACK的数据2是发送包的CRC32
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//每20ms重新发送一次REBOOT帧
	}
	return false;
}

//发送SIZE帧
//size：升级文件的总大小
//timout：超时时间，单位ms
bool STM32_IAP::send_SIZE(unsigned int size, int timeout)
{
	int count = 0;											//超时计算
	unsigned int tmp[4] = { 0 };							//要发送的数据
	tmp[0] = 0x04;										//长度
	tmp[1] = 0x05;										//功能字
	tmp[2] = size;										//升级文件大小
	tmp[3] = cal_crc(tmp, 3);							//CRC32
	sendData(QByteArray((char*)tmp, 4 * sizeof(int)));	//发送SIZE帧
	while (1)//等待返回ACK
	{	//如果收到了数据
		if (isReceiveReady)
		{	//功能字=ACK(0X01)且ACK的数据正确
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//收到包的功能字是ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK的数据1是发送包的功能字0x05
				((uint*)receiveBuffer.data())[3] == tmp[3])		//ACK的数据2是发送包的CRC32
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//每20ms重新发送一次SIZE帧
	}
	return false;
}

//发送BASEADDR帧
//addr：起始地址
//timout：超时时间，单位ms
bool STM32_IAP::send_BASEADDR(unsigned int addr, int timeout)
{
	int count = 0;
	unsigned int tmp[4] = { 0 };
	tmp[0] = 0x04;										//长度
	tmp[1] = 0x06;										//功能字
	tmp[2] = addr;										//起始地址
	tmp[3] = cal_crc(tmp, 3);							//CRC32
	sendData(QByteArray((char*)tmp, 4 * sizeof(int)));	//发送BASEADDR帧
	while (1)//等待返回ACK
	{
		if (isReceiveReady)
		{
			//功能字=ACK(0X01)且ACK的数据正确
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//收到包的功能字是ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK的数据1是发送包的功能字0x07
				((uint*)receiveBuffer.data())[3] == tmp[3])		//ACK的数据2是发送包的CRC32
			{	//如果计算出的CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return false;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//每20ms重新发送一次BASEADDR帧
	}
	return false;
}

//发送GETERASE帧
//timout：超时时间，单位ms
//返回值：FLASH擦除进度（0~100），如发送失败返回-1
int STM32_IAP::send_GETERASE(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x07;
	tmp[2] = cal_crc(tmp, 2);							//计算CRC32
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//发送GETERASE帧
	while (1)//等待返回ERASY帧
	{	//如果收到数据了
		if (isReceiveReady)
		{	//收到的数据是ERASY
			if (((uint*)receiveBuffer.data())[1] == 0x04)
			{	//CRC32正确
				if (cal_crc((uint*)receiveBuffer.data(), 3) == ((uint*)receiveBuffer.data())[3])
				{
					isReceiveReady = false;
					return ((uint*)receiveBuffer.data())[2];//返回收到的FLASH擦除进度
				}
			}
		}
		isReceiveReady = false;
		nmsl::e_system::delay_ms(1);		//延时1ms
		count++;							//延时计数+1
		if (count >= timeout) return -1;	//如果超时时间到了
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//每20ms重新发送一次GETERASE帧
	}
	return -1;
}















//计算CRC32，兼容STM32
unsigned int STM32_IAP::cal_crc(unsigned int* ptr, int len)
{
	unsigned int xbit;
	unsigned int data;
	unsigned int tmp = 0xFFFFFFFF;
	while (len--)
	{
		xbit = 1 << 31;
		data = *ptr++;
		for (int bits = 0; bits < 32; bits++) {
			if (tmp & 0x80000000) {
				tmp <<= 1;
				tmp ^= 0x04c11db7;
			}
			else
				tmp <<= 1;
			if (data & xbit)
				tmp ^= 0x04c11db7;

			xbit >>= 1;
		}
	}
	return tmp;
}


