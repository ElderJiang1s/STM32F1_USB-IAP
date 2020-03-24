#include "nmsl.h"

//系统_读取文件
//name：要读取的文件名
//返回值：读取到的文件数据
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

//系统_保存文件
//name：要保存的文件名
//data：要保存的文件数据
//isCover：有同名文件是否覆盖，默认不覆盖
//返回值：是否保存成功
bool nmsl::e_system::fileSave(QString name, QByteArray data, bool isCover)
{
	int tmp;
	QFile file(name);
	if (QFileInfo::exists(name) == false)//没有同名文件
	{
		file.open(QIODevice::NewOnly);
		tmp = file.write(data);
		file.close();
		if (tmp == -1)
			return false;//保存失败
		else
			return true;//保存成功
	}
	else//有同名文件
	{
		if (isCover)//覆盖同名文件
		{
			if (QFile::remove(name) == true)//删除同名文件再保存
			{
				file.open(QIODevice::NewOnly);
				tmp = file.write(data);
				file.close();
				if (tmp == -1)
					return false;//保存失败
				else
					return true;//保存成功
			}
			else
			{
				return false;//删除同名文件失败
			}
		}
		else//不覆盖
		{
			return false;
		}
	}
}

//系统_取屏幕分辨率
//colordepth：色深
//width：屏幕宽度
//height：屏幕高度
//efreshRate：屏幕刷新率
void nmsl::e_system::getDisplayInf(short& colordepth, short& width, short& height, short& refreshRate)
{
	short arr[61] = { 0 };
	EnumDisplaySettingsA(0, -1, (DEVMODEA*)arr);
	colordepth = arr[52];
	width = arr[54];
	height = arr[56];
	refreshRate = arr[60];

}

//系统_延时ms
//不占用cpu，窗口不卡死，不影响其它代码执行
//nms：要延时多少ms
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

//程序_是否被调试
//判断自身是否被调试；
bool nmsl::e_system::isdebug()
{
	return  IsDebuggerPresent();
}


//计算CRC32，支持STM32
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
