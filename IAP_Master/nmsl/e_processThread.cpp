#include "nmsl.h"



//����_����
//name������·��
//argv��������
//����ֵ������ID
int nmsl::e_process::processCreate(QString name, QString argv)
{
	QString a = name.left(name.lastIndexOf("\\"));
	STARTUPINFOA b;//���ڽṹ
	PROCESS_INFORMATION c;
	memset(&b, 0x0, sizeof(STARTUPINFOA));
	CreateProcessA(name.toUtf8().data(), argv.toUtf8().data(), 0, 0, 0, 0, 0, a.toUtf8().data(), &b, &c);
	return c.dwProcessId;
}

//����_��
//ͨ��Ŀ�괰�ھ��ȡ���̾�����ɹ��ͷ��ؽ��̾��
//windowHandle�����ھ��
int nmsl::e_process::processOpen(long long windowHandle)
{
	long long a;
	DWORD dwProcessID;
	GetWindowThreadProcessId((HWND)windowHandle, &dwProcessID);
	a = (long long)OpenProcess(2035711, 0, dwProcessID);
	return a;
}