#include "nmsl.h"



//进程_创建
//name：程序路径
//argv：命令行
//返回值：进程ID
int nmsl::e_process::processCreate(QString name, QString argv)
{
	QString a = name.left(name.lastIndexOf("\\"));
	STARTUPINFOA b;//窗口结构
	PROCESS_INFORMATION c;
	memset(&b, 0x0, sizeof(STARTUPINFOA));
	CreateProcessA(name.toUtf8().data(), argv.toUtf8().data(), 0, 0, 0, 0, 0, a.toUtf8().data(), &b, &c);
	return c.dwProcessId;
}

//进程_打开
//通过目标窗口句柄取进程句柄，成功就返回进程句柄
//windowHandle：窗口句柄
int nmsl::e_process::processOpen(long long windowHandle)
{
	long long a;
	DWORD dwProcessID;
	GetWindowThreadProcessId((HWND)windowHandle, &dwProcessID);
	a = (long long)OpenProcess(2035711, 0, dwProcessID);
	return a;
}