#include "nmsl.h"


//内部_Cookie取名
QString nmsl::e_inside::cookieGetName(QString cookie)
{
	int pos = cookie.indexOf("=");
	if (pos != -1)
		return cookie.left(pos);
	else
		return cookie;
}

//内部_Cookie取值
QString nmsl::e_inside::cookieGetVal(QString cookie)
{
	int pos;
	QString val;
	pos = cookie.indexOf("=");
	if (pos != -1)
		val = cookie.right(cookie.length() - pos - 1);
	return val;
}

//内部_协议头取名
QString nmsl::e_inside::headerGetName(QString header)
{
	int pos;
	QString val;
	pos = header.indexOf(":");
	if (pos != -1)
		val = header.left(pos);
	return val.trimmed();
}

//内部_协议头取值
QString nmsl::e_inside::headerGetVal(QString header)
{
	int pos;
	QString val;
	pos = header.indexOf(":");
	if (pos != -1)
		val = header.right(header.length() - pos - 1);
	return val;
}

//内部_数组成员是否存在1
//Cookie处理专用
//arr：Cookie数组
//val：要判断的Cookie名
//返回值：如果Cookie数组arr中有val返回true
bool nmsl::e_inside::stringListIsExistence1(QStringList arr, QString val)
{
	for (int i = 0; i < arr.length(); i++)
	{
		if (cookieGetName(arr[i]) == val)
			return true;
	}
	return false;
}

//内部_数组成员是否存在_文本
//arr：数组
//val：要判断值
//返回值：如果数组arr中存在val返回位置，如果不存在返回-1
int nmsl::e_inside::stringListIsExistence_Text(QStringList arr, QString val)
{
	for (int i = 0; i < arr.length(); i++)
	{
		if (arr[i] == val)
			return i;
	}
	return -1;
}

//内部_取单条Cookie1
//在网页_取单条Cookie中调用
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
