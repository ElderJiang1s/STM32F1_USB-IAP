#include "nmsl.h"



//取出中间文本
//text：欲取全文本
//befor：前面文本
//after：后面文本
//start_pos：起始搜寻位置，默认0
//cs：是否不区分大小写，默认不区分
QString nmsl::e_text::getMiddleString(QString text, QString befor, QString after, int start_pos, Qt::CaseSensitivity cs)
{
	int befor_pos = text.indexOf(befor, start_pos, cs);
	if (befor_pos != -1)
		befor_pos += befor.length();
	int after_pos = text.indexOf(after, befor_pos, cs);
	if (befor_pos == -1 || after_pos == -1)
		return QString();
	return text.mid(befor_pos, after_pos - befor_pos);
}


//文本_去重复文本
//text：原文本
//sep：分割符
QString nmsl::e_text::removeRepeatText(QString text, QString sep)
{
	QStringList arr;
	QStringList arr1;
	int i;
	QString tmp;

	arr = text.split(sep);
	for (i = 0; i < arr.length(); i++)
	{
		if (nmsl::e_inside::stringListIsExistence_Text(arr1, arr[i]) == -1)
		{
			arr1 << arr[i];
			tmp += arr[i] + sep;
		}
	}
	tmp = tmp.left(tmp.length() - sep.length());
	return tmp;
}

//文本_取左边
//text：被查找的文本
//find：欲寻找的文本
//start_pos：起始寻找位置，默认为0
//cs：是否不区分大小写，默认不区分
QString nmsl::e_text::getLeft(QString text, QString find, int start_pos, Qt::CaseSensitivity cs)
{
	int pos;
	QString tmp = "";
	if (start_pos < 0)
		start_pos = 0;
	pos = text.indexOf(find, start_pos, cs);
	if (pos == -1)
		return tmp;
	tmp = text.left(pos);
	return tmp;
}



//文本_取右边
//text：被查找的文本
//find：欲寻找的文本
//start_pos：起始寻找位置，如果本参数被省略，默认为从被搜寻文本的尾部开始
//cs：是否不区分大小写，默认不区分
QString nmsl::e_text::getRight(QString text, QString find, int start_pos, Qt::CaseSensitivity cs)
{
	int pos;
	QString tmp = "";
	start_pos = (start_pos == -1) ? -1 : start_pos;
	pos = text.lastIndexOf(find, start_pos, cs);
	if (pos == -1)
		return tmp;
	tmp = text.right(text.length() - find.length() - pos);
	return tmp;
}

