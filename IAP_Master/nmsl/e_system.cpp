#include "nmsl.h"

//ϵͳ_��ȡ�ļ�
//name��Ҫ��ȡ���ļ���
//����ֵ����ȡ�����ļ�����
QByteArray nmsl::e_system::fileLoad(QString name)
{
	QByteArray data = QByteArray();
	QFile file(name);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();
		return data;
	}
	data = file.readAll();
	file.close();
	return data;
}

//ϵͳ_�����ļ�
//name��Ҫ������ļ���
//data��Ҫ������ļ�����
//isCover����ͬ���ļ��Ƿ񸲸ǣ�Ĭ�ϲ�����
//����ֵ���Ƿ񱣴�ɹ�
bool nmsl::e_system::fileSave(QString name, QByteArray data, bool isCover)
{
	int tmp;
	QFile file(name);
	if (QFileInfo::exists(name) == false)//û��ͬ���ļ�
	{
		file.open(QIODevice::NewOnly);
		tmp = file.write(data);
		file.close();
		if (tmp == -1)
			return false;//����ʧ��
		else
			return true;//����ɹ�
	}
	else//��ͬ���ļ�
	{
		if (isCover)//����ͬ���ļ�
		{
			if (QFile::remove(name) == true)//ɾ��ͬ���ļ��ٱ���
			{
				file.open(QIODevice::NewOnly);
				tmp = file.write(data);
				file.close();
				if (tmp == -1)
					return false;//����ʧ��
				else
					return true;//����ɹ�
			}
			else
			{
				return false;//ɾ��ͬ���ļ�ʧ��
			}
		}
		else//������
		{
			return false;
		}
	}
}

//ϵͳ_ȡ��Ļ�ֱ���
//colordepth��ɫ��
//width����Ļ���
//height����Ļ�߶�
//efreshRate����Ļˢ����
void nmsl::e_system::getDisplayInf(short& colordepth, short& width, short& height, short& refreshRate)
{
	short arr[61] = { 0 };
	EnumDisplaySettingsA(0, -1, (DEVMODEA*)arr);
	colordepth = arr[52];
	width = arr[54];
	height = arr[56];
	refreshRate = arr[60];

}

//ϵͳ_��ʱms
//��ռ��cpu�����ڲ���������Ӱ����������ִ��
//nms��Ҫ��ʱ����ms
bool nmsl::e_system::delay_ms(int nms)
{
	LARGE_INTEGER t;
	HANDLE t_handle;
	t.QuadPart = -10 * nms * 1000 * 1;
	t_handle = CreateWaitableTimerA(0, false, 0);
	SetWaitableTimer(t_handle, &t, 0, 0, 0, false);
	while (MsgWaitForMultipleObjects(1, &t_handle, false, -1, 255) != 0)
		QApplication::processEvents();
	CloseHandle(t_handle);
	return true;
}

//����_�Ƿ񱻵���
//�ж������Ƿ񱻵��ԣ�
bool nmsl::e_system::isdebug()
{
	return  IsDebuggerPresent();
}


//����CRC32��֧��STM32
unsigned int nmsl::e_system::cal_crc(unsigned int* ptr, int len)
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
