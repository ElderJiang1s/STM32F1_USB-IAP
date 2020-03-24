#include "nmsl.h"


//�ڲ�_Cookieȡ��
QString nmsl::e_inside::cookieGetName(QString cookie)
{
	int pos = cookie.indexOf("=");
	if (pos != -1)
		return cookie.left(pos);
	else
		return cookie;
}

//�ڲ�_Cookieȡֵ
QString nmsl::e_inside::cookieGetVal(QString cookie)
{
	int pos;
	QString val;
	pos = cookie.indexOf("=");
	if (pos != -1)
		val = cookie.right(cookie.length() - pos - 1);
	return val;
}

//�ڲ�_Э��ͷȡ��
QString nmsl::e_inside::headerGetName(QString header)
{
	int pos;
	QString val;
	pos = header.indexOf(":");
	if (pos != -1)
		val = header.left(pos);
	return val.trimmed();
}

//�ڲ�_Э��ͷȡֵ
QString nmsl::e_inside::headerGetVal(QString header)
{
	int pos;
	QString val;
	pos = header.indexOf(":");
	if (pos != -1)
		val = header.right(header.length() - pos - 1);
	return val;
}

//�ڲ�_�����Ա�Ƿ����1
//Cookie����ר��
//arr��Cookie����
//val��Ҫ�жϵ�Cookie��
//����ֵ�����Cookie����arr����val����true
bool nmsl::e_inside::stringListIsExistence1(QStringList arr, QString val)
{
	for (int i = 0; i < arr.length(); i++)
	{
		if (cookieGetName(arr[i]) == val)
			return true;
	}
	return false;
}

//�ڲ�_�����Ա�Ƿ����_�ı�
//arr������
//val��Ҫ�ж�ֵ
//����ֵ���������arr�д���val����λ�ã���������ڷ���-1
int nmsl::e_inside::stringListIsExistence_Text(QStringList arr, QString val)
{
	for (int i = 0; i < arr.length(); i++)
	{
		if (arr[i] == val)
			return i;
	}
	return -1;
}

//�ڲ�_ȡ����Cookie1
//����ҳ_ȡ����Cookie�е���
QString nmsl::e_inside::getlinecookie1(QString allcookie, QString cookiename)
{
	QStringList cookies;
	int i;
	QString name;
	cookies = allcookie.split(";");
	for (i = 0; i < cookies.length(); i++)
	{
		name = nmsl::e_text::getLeft(cookies[i].trimmed(), "=");
		if (name.toLower() == cookiename.toLower())
		{
			if (cookies[i].trimmed().indexOf(":") != -1)
				return nmsl::e_text::getMiddleString(cookies[i].trimmed(), "=", ":");
			return nmsl::e_text::getRight(cookies[i].trimmed(), "=");
		}
	}
	return "";
}
