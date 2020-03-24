#include "nmsl.h"



//ȡ���м��ı�
//text����ȡȫ�ı�
//befor��ǰ���ı�
//after�������ı�
//start_pos����ʼ��Ѱλ�ã�Ĭ��0
//cs���Ƿ����ִ�Сд��Ĭ�ϲ�����
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


//�ı�_ȥ�ظ��ı�
//text��ԭ�ı�
//sep���ָ��
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

//�ı�_ȡ���
//text�������ҵ��ı�
//find����Ѱ�ҵ��ı�
//start_pos����ʼѰ��λ�ã�Ĭ��Ϊ0
//cs���Ƿ����ִ�Сд��Ĭ�ϲ�����
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



//�ı�_ȡ�ұ�
//text�������ҵ��ı�
//find����Ѱ�ҵ��ı�
//start_pos����ʼѰ��λ�ã������������ʡ�ԣ�Ĭ��Ϊ�ӱ���Ѱ�ı���β����ʼ
//cs���Ƿ����ִ�Сд��Ĭ�ϲ�����
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

