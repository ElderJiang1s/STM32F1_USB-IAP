//#include "STM32_IAP.h"
#include "app1.h"

//p��app1.cpp��app1ʵ����ָ��
//����������ڿؼ�
//Ҳ�ܵ���app1����ĺ���
STM32_IAP::STM32_IAP(app1* p)
{
	app = p;
}


//���ڽ��յ������ݣ�һ��������64�ֽڣ�����64�ֽڵ�����Ҫ��ν���
void STM32_IAP::receiveData(QByteArray data)
{
	receiveBuffer = data;
	isReceiveReady = true;
}

//���ڷ�������
void STM32_IAP::sendData(QByteArray data)
{
	app->sendData(data);
}

//����STM32����ȡӲ��ID������汾
//t:���Դ���
bool STM32_IAP::connect(int t)
{
	QVector<unsigned int> data;
	app->ui.progressBar->setValue(0);
	while (t--)
	{
		if (send_GETINF(data, 100))
		{
			userID = QString("%1").arg(data[2], 8, 16, QLatin1Char('0'));//uint32תstring��ǰ�油0
			userID += QString("%1").arg(data[1], 8, 16, QLatin1Char('0'));
			userID += QString("%1").arg(data[0], 8, 16, QLatin1Char('0'));
			BLversion = QString("%1").arg(data[3]);
			APPversion = QString("%1").arg(data[4]);
			app->ui.label_3->setText("�豸������ ID:");
			app->ui.label_stm32ID->setText(userID);
			app->ui.label_blVersion->setText(BLversion);
			app->ui.label_appVersion->setText(APPversion);
			return true;
		}
	}
	return false;
}


//���������ļ�
void STM32_IAP::loadUpdataFile(QByteArray file)
{
	updateFile = file;
	if (updateFile.size() % 4)//��������ļ�����4��������
	{
		updateFile += QByteArray(4 - (updateFile.size() % 4), 0);//�ļ�ĩβ���0
	}
}


//����STM32�����ڽ���������ʾ����
bool STM32_IAP::erase()
{
	int error_count = 0;
	int val = 0;
	if (send_SIZE(updateFile.size(), 100) == false)	//���������ļ���С
	{
		cout << "send_SIZE FAIL";
		return false;
	}
	cout << "send_SIZE OK";

	if (send_BASEADDR(0xF000, 100) == false)//������ʼ��ַ0x800F000��STM32����ACK��ʼ���� boootloader 60KB
	{
		cout << "send_BASEADDR FAIL";	//BASEADDR����ʧ��
		return false;
	}
	cout << "send_BASEADDR OK";			//BASEADDR���ͳɹ�

	app->ui.btn_startUpdate->setText("���ڲ���...");
	app->ui.progressBar->setValue(0);	//��������0
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
		if (error_count > 100 || val == 100)//������󳬹�100����������
		{
			break;
		}
	}
	if (error_count > 100)	//������󳬹�100
	{
		app->ui.btn_startUpdate->setText("����ʧ��");
		cout << "send_ERASE FAIL";
		return false;
	}
	if (val == 100)		//��������ɹ�
	{
		app->ui.btn_startUpdate->setText("�����ɹ�");
		cout << "send_ERASE OK";
	}
	return true;
}


bool STM32_IAP::Programming()
{
	int i;
	app->ui.btn_startUpdate->setText("��������...");
	app->ui.progressBar->setValue(0);	//��������0

	for (i = 0; i < updateFile.length() / 1024; i++)				//���������ļ���ÿһ����1024byte����
	{
		if (send_UPDATE(updateFile.mid(i * 1024, 1024), i, 500))
		{
			cout << "send_UPDATE" << i << "OK";
			app->ui.progressBar->setValue((int)((float)i / (updateFile.length() / 1024 + 1) * 100));
		}
		else
		{
			cout << "send_UPDATE" << i << "FAIL";
			app->ui.btn_startUpdate->setText("����ʧ��");
			return false;
		}
	}
	if (updateFile.length() % 1024)									//�������һ������1024byte������
	{
		if (send_UPDATE(updateFile.mid(i * 1024), i, 500))
		{
			cout << "send_UPDATE" << i << "OK";
			app->ui.progressBar->setValue(100);

		}
		else
		{
			cout << "send_UPDATE" << i << "FAIL";
			app->ui.btn_startUpdate->setText("����ʧ��");
			return false;
		}
	}
	app->ui.btn_startUpdate->setText("�����ɹ�");
	return true;
}


//����˿�ʼ������ť
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
	QMessageBox::information(app, "�����ɹ�", QString("��������ɹ�\r\n�ٶȣ�%1 KB/S").arg((float)updateFile.size()/t));
}

//����GETINF֡
//data[0:2]��STM32���к�
//data[3]��BL�汾
//data[4]��APP�汾
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_GETINF(QVector<unsigned int>& data, int timeout)
{
	int count = 0;										//��ʱ����
	unsigned int tmp[3] = { 0 };						//Ҫ���͵�����
	tmp[0] = 0x03;										//����
	tmp[1] = 0x01;										//������
	tmp[2] = cal_crc(tmp, 2);							//����CRC32
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//����GETINF֡
	data.clear();										//���data������Ҫװ����
	while (1)//�ȴ�����INF֡
	{	//����յ�������
		if (isReceiveReady)
		{	//�յ���������INF
			if (((uint*)receiveBuffer.data())[1] == 0x02)
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 7) == ((uint*)receiveBuffer.data())[7])
				{
					data << ((uint*)receiveBuffer.data())[2];//STM32���к�1
					data << ((uint*)receiveBuffer.data())[3];//STM32���к�2
					data << ((uint*)receiveBuffer.data())[4];//STM32���к�3
					data << ((uint*)receiveBuffer.data())[5];//BL�汾
					data << ((uint*)receiveBuffer.data())[6];//APP�汾
					isReceiveReady = false;//��������Ѵ���
					return true;
				}
			}
			isReceiveReady = false;			//���CRC32����ȷҲ��������Ѵ����ȴ������ٴε���
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//ÿ20ms���·���һ��GETINF֡
	}
	return false;
}

//����UPDATE֡
//data��Ҫ���͵��ļ����ݣ����1024byte
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_UPDATE(QByteArray data, uint t, int timeout)
{
	int count = 0;
	unsigned int tmp[259] = { 0 };
	tmp[0] = 4 + (data.length() >> 2);							//����
	tmp[1] = 0x02;												//������
	tmp[2] = t;													//���ݰ������
	for (int i = 0; i < (data.length() >> 2); i++)				//����
		tmp[3 + i] = ((uint*)data.data())[i];					///*  + ���ȼ��� >> ��  */
	tmp[3 + (data.length() >> 2)] = cal_crc(tmp, 3 + (data.length() >> 2));	//CRC32
	sendData(QByteArray((char*)tmp, (4 + (data.length() >> 2)) * sizeof(int)));	//��������
	while (1)//�ȴ�����ACK
	{	//����յ�������
		if (isReceiveReady)
		{	//������=ACK(0X01)��ACK��������ȷ
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&							//�յ����Ĺ�������ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&						//ACK������1�Ƿ��Ͱ��Ĺ�����0x02
				((uint*)receiveBuffer.data())[3] == tmp[3 + (data.length() >> 2)] &&	//ACK������2�Ƿ��Ͱ���CRC32
				((uint*)receiveBuffer.data())[4] == tmp[2])							//ACK������3�Ƿ��Ͱ����
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		//if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//ÿ20ms���·���һ��SIZE֡
	}
	return true;
}

//����FINISH֡
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_FINISH(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x03;
	tmp[2] = cal_crc(tmp, 2);
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//����FINISH֡
	while (1)//�ȴ�����ACK
	{	//����յ�������
		if (isReceiveReady)
		{	//������=ACK(0X01)��ACK��������ȷ
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//�յ����Ĺ�������ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK������1�Ƿ��Ͱ��Ĺ�����0x04
				((uint*)receiveBuffer.data())[3] == tmp[2])		//ACK������2�Ƿ��Ͱ���CRC32
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//ÿ20ms���·���һ��FINISH֡
	}
	return false;
}

//����REBOOT֡
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_REBOOT(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x04;
	tmp[2] = cal_crc(tmp, 2);
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//����REBOOT֡
	while (1)//�ȴ�����ACK
	{	//����յ�������
		if (isReceiveReady)
		{	//������=ACK(0X01)��ACK��������ȷ
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//�յ����Ĺ�������ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK������1�Ƿ��Ͱ��Ĺ�����0x04
				((uint*)receiveBuffer.data())[3] == tmp[2])		//ACK������2�Ƿ��Ͱ���CRC32
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//ÿ20ms���·���һ��REBOOT֡
	}
	return false;
}

//����SIZE֡
//size�������ļ����ܴ�С
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_SIZE(unsigned int size, int timeout)
{
	int count = 0;											//��ʱ����
	unsigned int tmp[4] = { 0 };							//Ҫ���͵�����
	tmp[0] = 0x04;										//����
	tmp[1] = 0x05;										//������
	tmp[2] = size;										//�����ļ���С
	tmp[3] = cal_crc(tmp, 3);							//CRC32
	sendData(QByteArray((char*)tmp, 4 * sizeof(int)));	//����SIZE֡
	while (1)//�ȴ�����ACK
	{	//����յ�������
		if (isReceiveReady)
		{	//������=ACK(0X01)��ACK��������ȷ
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//�յ����Ĺ�������ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK������1�Ƿ��Ͱ��Ĺ�����0x05
				((uint*)receiveBuffer.data())[3] == tmp[3])		//ACK������2�Ƿ��Ͱ���CRC32
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//ÿ20ms���·���һ��SIZE֡
	}
	return false;
}

//����BASEADDR֡
//addr����ʼ��ַ
//timout����ʱʱ�䣬��λms
bool STM32_IAP::send_BASEADDR(unsigned int addr, int timeout)
{
	int count = 0;
	unsigned int tmp[4] = { 0 };
	tmp[0] = 0x04;										//����
	tmp[1] = 0x06;										//������
	tmp[2] = addr;										//��ʼ��ַ
	tmp[3] = cal_crc(tmp, 3);							//CRC32
	sendData(QByteArray((char*)tmp, 4 * sizeof(int)));	//����BASEADDR֡
	while (1)//�ȴ�����ACK
	{
		if (isReceiveReady)
		{
			//������=ACK(0X01)��ACK��������ȷ
			if (((uint*)receiveBuffer.data())[1] == 0x01 &&		//�յ����Ĺ�������ACK
				((uint*)receiveBuffer.data())[2] == tmp[1] &&	//ACK������1�Ƿ��Ͱ��Ĺ�����0x07
				((uint*)receiveBuffer.data())[3] == tmp[3])		//ACK������2�Ƿ��Ͱ���CRC32
			{	//����������CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 5) == ((uint*)receiveBuffer.data())[5])
				{
					isReceiveReady = false;
					return true;
				}
			}
			isReceiveReady = false;
		}
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return false;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 4 * sizeof(int)));//ÿ20ms���·���һ��BASEADDR֡
	}
	return false;
}

//����GETERASE֡
//timout����ʱʱ�䣬��λms
//����ֵ��FLASH�������ȣ�0~100�����緢��ʧ�ܷ���-1
int STM32_IAP::send_GETERASE(int timeout)
{
	int count = 0;
	unsigned int tmp[3] = { 0 };
	tmp[0] = 0x03;
	tmp[1] = 0x07;
	tmp[2] = cal_crc(tmp, 2);							//����CRC32
	sendData(QByteArray((char*)tmp, 3 * sizeof(int)));	//����GETERASE֡
	while (1)//�ȴ�����ERASY֡
	{	//����յ�������
		if (isReceiveReady)
		{	//�յ���������ERASY
			if (((uint*)receiveBuffer.data())[1] == 0x04)
			{	//CRC32��ȷ
				if (cal_crc((uint*)receiveBuffer.data(), 3) == ((uint*)receiveBuffer.data())[3])
				{
					isReceiveReady = false;
					return ((uint*)receiveBuffer.data())[2];//�����յ���FLASH��������
				}
			}
		}
		isReceiveReady = false;
		nmsl::e_system::delay_ms(1);		//��ʱ1ms
		count++;							//��ʱ����+1
		if (count >= timeout) return -1;	//�����ʱʱ�䵽��
		if ((count % 20) == 0) sendData(QByteArray((char*)tmp, 3 * sizeof(int)));//ÿ20ms���·���һ��GETERASE֡
	}
	return -1;
}















//����CRC32������STM32
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


