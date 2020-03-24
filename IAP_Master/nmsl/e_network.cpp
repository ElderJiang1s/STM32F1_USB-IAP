#include "nmsl.h"


//��ҳ_ȡ����
//ͨ����/�����ߡ�.������ȡ��ַ�е�������Ĭ��ʹ�á�/����ʽ
//url����ַ
//rem2Domain���Ƿ�ȥ����������
//nofilterPort���Ƿ񲻹��˶˿�
//pointNum����׺�������������ձ�ʾͨ����.���ķ�ʽȡ���� ���磺.com��׺������Ϊ1 ,.com.cn��׺��
QString nmsl::e_network::getDomain(QString url, bool rem2Domain, bool nofilterPort, int pointNum)
{
	QString domain;			//����
	int pos;				//λ��
	QStringList twoLevel;	//����
	int domainLen;			//��������
	int frontPointStart;	//ǰ�㿪ʼ,��www.cf.qq.com   www����.��λ��
	int frontPointEnd;		//ǰ�����,��www.cf.qq.com   cf����.��λ��
	int afterPointStart;	//��㿪ʼ,��www.cf.qq.com   qq����.��λ��

	domain = url.toLower();//��Сд
	if (pointNum == -1)//ʹ�á�/����ʽ��ȡ����
	{
		if (domain.right(1) != "/")
			domain += "/";
		if (domain.left(8) == "https://")
		{
			domain = nmsl::e_text::getMiddleString(domain, "https://", "/");
		}
		else
		{
			if (domain.indexOf("http://", 0) == -1)
				domain = "http://" + domain;
			domain = nmsl::e_text::getMiddleString(domain, "http://", "/");
		}

		if (rem2Domain)
		{
			twoLevel = domain.split(".");//��"."�ָ�
			pos = twoLevel.length();
			if (pos > 1)
			{
				domain = twoLevel[pos - 2] + "." + twoLevel[pos - 1];
			}
		}
	}
	else//ͨ����.���ķ�ʽȡ������֧���κκ�׺����
	{
		domain = domain.replace("http://", "");//��domain��"http://"�滻Ϊ""
		domain = domain.replace("https://", "");
		pos = domain.indexOf("/");
		if (pos != -1)
			domain = domain.left(pos);
		domainLen = domain.length();		//��������
		afterPointStart = domain.length() - 1;//��㿪ʼ
		for (int i = 0; i < pointNum; i++)
			afterPointStart = domain.lastIndexOf(".", afterPointStart);
		frontPointEnd = domain.lastIndexOf(".", afterPointStart);
		if (rem2Domain)
		{
			domain = domain.right(domainLen - frontPointEnd);
		}
		else//������������
		{
			if (frontPointEnd != -1)//�����Ƕ�������
			{
				frontPointStart = domain.lastIndexOf(".", frontPointEnd);
				if (frontPointEnd != -1)//��������������
				{
					domain = domain.right(domainLen - frontPointStart + 1);
				}
			}
		}
	}
	//�˿ڹ��˴���--------------------------------------------------------
	if (nofilterPort)
	{
		return domain;
	}
	else
	{
		pos = domain.indexOf(":");
		if (pos != -1)
			return domain.left(pos);
		else
			return domain;
	}
}

//��ҳ_ȡ�˿�
//ȡ��url��Ĭ�϶˿�
int nmsl::e_network::getPort(QString url)
{
	QString port;
	int pos;
	port = getDomain(url, false, true);
	pos = port.lastIndexOf(":");

	if (pos != -1)
	{
		port = port.right(port.length() - pos - 1);
		cout << "port = " << port;
		return port.toInt();
	}
	else
	{
		if (url.toLower().left(5) == "https")
			return 443;
		else
			return 80;
	}
}

//��ҳ_ȡҳ���ַ
QString nmsl::e_network::getUrlAddr(QString url)
{
	QString urlAddr;	//ҳ���ַ
	int pos;			//����λ��
	QString domain;		//����
	domain = getDomain(url);
	pos = url.indexOf("/", url.indexOf(domain));
	if (pos > 0)
		urlAddr = url.right(url.length() - pos);
	else
		urlAddr = "/";
	return(urlAddr);
}

//��ҳ_Cookie�ϲ�����
//���ظ��º��Cookie
QString nmsl::e_network::cookieUpdate(QString& oldCookie, QString newCookie)
{
	QStringList oldCookie_List;
	QStringList newCookie_List;
	int count;
	//������ʽ��
	oldCookie.remove(QRegExp("\\s"));//ɾȫ����
	newCookie.remove(QRegExp("\\s"));//ɾȫ����
	//��ʼ����Cookie
	oldCookie_List = oldCookie.split(";");
	newCookie_List = newCookie.split(";");
	for (int i = 0; i < oldCookie_List.length(); i++)
	{
		if (nmsl::e_inside::stringListIsExistence1(newCookie_List, nmsl::e_inside::cookieGetName(oldCookie_List[i])) == false)
			newCookie_List << oldCookie_List[i];
	}
	//ȥ����Ч��Cookie
	oldCookie = "";
	for (int i = 0; i < newCookie_List.length(); i++)
	{
		if (newCookie_List[i].right(8) != "=deleted")
			oldCookie += newCookie_List[i] + "; ";
	}
	oldCookie = oldCookie.left(oldCookie.length() - 2);
	//���ظ���Cookie���кϲ�
	oldCookie = nmsl::e_text::removeRepeatText(oldCookie, "; ");
	return oldCookie;
}



//��ҳ_ȡ����Cookie
//cookie��ȫ��cookie
//name��Ҫ��ȡ��cookie����
//noName���Ƿ񲻸������ƣ�Ĭ�ϸ�������
QString nmsl::e_network::getLineCookie(QString cookie, QString name, bool noName)
{
	QString t;
	cookie = cookie.trimmed();//ɾ��β��
	if (cookie.right(1) != ";")
		cookie += ";";
	t = nmsl::e_inside::getlinecookie1(cookie, name);
	if (noName)
		return t;
	else
		return name + "=" + t;
}

//��ҳ_ȡCookie
//domain��ֻ��������
//cookieName����Cookie���ƣ�Ϊ�շ���ȫ��Cookie
QString nmsl::e_network::getCookie(QString domain, QString cookieName)
{
	QString  url;
	QString val;
	char tmp[3000] = { 0 };
	char tmp1[3000] = { 0 };
	url = "http://" + domain;
	int s = 3000;
	InternetGetCookieA(url.toUtf8().data(), (char)0, tmp, (LPDWORD)&s);
	s = 3000;
	InternetGetCookieExA(url.toUtf8().data(), (char)0, tmp1, (LPDWORD)&s, 8192, 0);
	val = QString(tmp);
	e_network::cookieUpdate(val, QString(tmp1));//Cookie�ϲ�����
	if (cookieName != "")
	{

	}
	return val;
}

//��ҳ_ȡ����IP
//pos������ʡ��
QString nmsl::e_network::getExtIP(QString& pos)
{
	QString source;
	source = http_access("https://2020.ip138.com/");
	source = nmsl::e_text::getMiddleString(source, "����iP", "</p>");
	pos = nmsl::e_text::getMiddleString(source, "���ԣ�", "\r\n");
	return nmsl::e_text::getMiddleString(source, "��ַ�ǣ�[", "] ���ԣ�");
}

//��ҳ_��ָ����ַ
//ʹ��ϵͳĬ�ϵ��������һ����ַ
//url��Ҫ�򿪵���ҳ��ַ
void nmsl::e_network::openUrl(QString url)
{
	ShellExecuteA(0, "open", url.toUtf8().data(), "", "", 1);
}

//��ҳ_���Cookie
//domain��ֻ��������
void nmsl::e_network::clearCookie(QString domain)
{
	QString cookieVal1;
	QString cookieVal2;
	QStringList cookie;
	int count;
	QString finalCookie;
	QString cookieName;

	cookieVal1 = "deleted; expires=Fri, 1-Jan-1999 1:1:1 GMT; path=/;";
	cookieVal2 = cookieVal1 + " domain=" + getDomain(domain, true);
	finalCookie = getCookie(domain);
	cookie = finalCookie.split(";");
	for (count = 0; count < cookie.length(); count++)
	{
		cookieName = nmsl::e_inside::cookieGetName(cookie[count]);
		InternetSetCookieA(("http://" + domain).toUtf8().data(), cookieName.toUtf8().data(), cookieVal1.toUtf8().data());
		InternetSetCookieA(("http://" + domain).toUtf8().data(), cookieName.toUtf8().data(), cookieVal2.toUtf8().data());
	}
}



//��ҳ_����
//ʹ��WinInet�ķ�ʽ������ҳ
QByteArray nmsl::e_network::http_access(
	QString url,					//��������ҳ��ַ,�������http://����https://
	int type,						//0=GET 1=POST 2=HEAD
	QString postData,				//POSTר��
	QString& sendCookies,			//�ύCookies
	QString& returnCookies,			//���ص�Cookie
	QString headers,				//����Э��ͷ
	QString& returnHeaders,			//����Э��ͷ
	bool disableRedirection,		//��ֹ�ض���,Ĭ�ϲ���ֹ��ҳ�ض���
	QByteArray sendByteArrar,		//�ύByteArray����
	QString privoxy)				//�����ַ����ʽΪ 8.8.8.8:88
{
	HINTERNET internetHandle;		//��_Internet���
	HINTERNET internetConnectHandle;//��_Internet���Ӿ��
	HINTERNET httpReqHandle;		//��_HTTP������
	int count;						//��_�ƴ�
	//QByteArray ByteArray_tmp;		//��_�ֽڼ�
	QString req_type;				//��_���ʷ�ʽ
	QString User_Agent;				//��_User_Agent
	QStringList header_return;		//��_����Э��ͷ
	QByteArray	content;			//��_ҳ������
	unsigned int reqSign;			//��_������
	QString redirectionAddr;		//��_�ض����ַ
	bool isHttps;					//��_HTTPS


	if (type < 0 || type>2)
		type = 0;

	switch (type)
	{
	case 0: req_type = "GET"; break;
	case 1: req_type = "POST"; break;
	case 2: req_type = "HEAD"; break;
	}

	if (url.mid(0, 5) == "https" || url.mid(0, 5) == "HTTPS")
		isHttps = true;
	else
		isHttps = false;

	if (headers.indexOf("User-Agent:", 0, Qt::CaseInsensitive) != -1)//�����������Э��ͷ����"User-Agent:"
	{
		User_Agent = nmsl::e_text::getMiddleString(headers, "User-Agent:", "\r\n");//�Ѳ�������Э��ͷ���"User-Agent:"��User_Agent
		if (User_Agent == "")
		{
			if (headers.indexOf(":") != -1)
				User_Agent = headers.right(headers.length() - headers.indexOf(":"));
		}
	}
	else
	{
		User_Agent = "Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)";//���û�о��Լ�ָ��һ��
	}

	if (privoxy == "")
	{
		internetHandle = InternetOpenA(User_Agent.toUtf8().data(), 1, (char)0, (char)0, 0);
	}
	else
	{
		if (isHttps)
			internetHandle = InternetOpenA(User_Agent.toUtf8().data(), 3, privoxy.toUtf8().data(), "", 0);
		else
			internetHandle = InternetOpenA(User_Agent.toUtf8().data(), 3, ("http=" + privoxy).toUtf8().data(), "", 0);
	}

	if (internetHandle == 0)
	{
		InternetCloseHandle(internetHandle);
		return QByteArray();
	}
	internetConnectHandle = InternetConnectA(internetHandle, getDomain(url).toUtf8().data(), getPort(url), (char)0, (char)0, 3, 0, 0);
	if (internetConnectHandle == 0)
	{
		InternetCloseHandle(internetConnectHandle);
		InternetCloseHandle(internetHandle);
		return QByteArray();
	}

	reqSign = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;//��_������
	if (sendCookies != "")//����������ṩ��Cookies
		reqSign |= INTERNET_FLAG_NO_COOKIES;

	if (disableRedirection)//�����ֹ�ض���
		reqSign |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	if (isHttps)//�Ƿ�https
		reqSign |= INTERNET_FLAG_SECURE;
	else
		reqSign |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;

	//��_HTTP������
	httpReqHandle = HttpOpenRequestA(internetConnectHandle, req_type.toUtf8().data(), getUrlAddr(url).toUtf8().data(),
		QString("HTTP/1.1").toUtf8().data(), (char)0, (char)0, reqSign, 0);
	if (httpReqHandle == 0)
	{
		InternetCloseHandle(httpReqHandle);
		InternetCloseHandle(internetConnectHandle);
		InternetCloseHandle(internetHandle);
		return QByteArray();
	}

	if (headers == "")//����Э��ͷ
	{
		headers = "Accept: */*";
	}
	else
	{
		if (headers.indexOf("Accept:") == -1)
		{
			headers += "\r\n";
			headers += "Accept: */*";
		}
	}

	if (headers.indexOf("Referer:") == -1)
	{
		headers += "\r\n";
		headers += "Referer:" + url;
	}

	if (headers.indexOf("Accept-Language:") == -1)
	{
		headers += "\r\n";
		headers += "Accept-Language: zh-cn";
	}

	if (headers.indexOf("Content-Type:") == -1)
	{
		headers += "\r\n";
		headers += "Content-Type: application/x-www-form-urlencoded";
	}

	if (sendCookies != "")//�ύCookies
	{
		headers += "\r\n";
		headers += "Cookie:" + sendCookies;
	}

	if (type == 0)
	{
		HttpSendRequestA(httpReqHandle, headers.toUtf8().data(), headers.length(), "", 0);
	}
	else
	{
		if (sendByteArrar == QByteArray())
			sendByteArrar = postData.toUtf8();
		HttpSendRequestA(httpReqHandle, headers.toUtf8().data(), headers.length(), sendByteArrar.data(), sendByteArrar.length());
	}
	count = 0;//��_�ƴ�
	//ByteArray_tmp = QByteArray(1024, (char)0);
	char ByteArray_tmp[1024] = { 0 };
	do
	{
		InternetReadFile(httpReqHandle, ByteArray_tmp, 1024, (LPDWORD)&count);
		content += QByteArray(ByteArray_tmp, count);
	} while (count != 0);

	int returnHeadersSize_tmp = 5000;
	char returnHeaders_tmp[5000] = { 0 };
	HttpQueryInfoA(httpReqHandle, 22, returnHeaders_tmp, (LPDWORD)&returnHeadersSize_tmp, 0);
	returnHeaders = QString(QByteArray(returnHeaders_tmp, returnHeadersSize_tmp));

	InternetCloseHandle(httpReqHandle);
	InternetCloseHandle(internetConnectHandle);
	InternetCloseHandle(internetHandle);

	header_return = returnHeaders.split("\r\n");
	returnCookies = "";
	for (int i = 0; i < header_return.length(); i++)
	{
		if (header_return[i].indexOf("Set-Cookie") != -1)
		{
			if (header_return[i].indexOf(";") != -1)
				returnCookies += nmsl::e_text::getMiddleString(header_return[i], "Set-Cookie", ";") + ";";
			else
				returnCookies += header_return[i].replace("Set-Cookie:", "") + ";";
		}
	}
	returnCookies = returnCookies.left(returnCookies.length() - 2);

	if (sendCookies == "")//���û���ṩ����sendCookies
	{
		cookieUpdate(sendCookies, returnCookies);//��ҳ_Cookie�ϲ�����
	}
	return content;
}


