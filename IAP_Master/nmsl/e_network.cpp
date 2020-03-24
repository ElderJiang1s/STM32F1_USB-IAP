#include "nmsl.h"


//网页_取域名
//通过“/”或者“.”来获取网址中的域名，默认使用“/”方式
//url：网址
//rem2Domain：是否去除二级域名
//nofilterPort：是否不过滤端口
//pointNum：后缀点数量，不留空表示通过“.”的方式取域名 。如：.com后缀点数量为1 ,.com.cn后缀点
QString nmsl::e_network::getDomain(QString url, bool rem2Domain, bool nofilterPort, int pointNum)
{
	QString domain;			//域名
	int pos;				//位置
	QStringList twoLevel;	//二级
	int domainLen;			//域名长度
	int frontPointStart;	//前点开始,如www.cf.qq.com   www后面.的位置
	int frontPointEnd;		//前点结束,如www.cf.qq.com   cf后面.的位置
	int afterPointStart;	//后点开始,如www.cf.qq.com   qq后面.的位置

	domain = url.toLower();//到小写
	if (pointNum == -1)//使用“/”方式获取域名
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
			twoLevel = domain.split(".");//按"."分割
			pos = twoLevel.length();
			if (pos > 1)
			{
				domain = twoLevel[pos - 2] + "." + twoLevel[pos - 1];
			}
		}
	}
	else//通过“.”的方式取域名，支持任何后缀域名
	{
		domain = domain.replace("http://", "");//把domain中"http://"替换为""
		domain = domain.replace("https://", "");
		pos = domain.indexOf("/");
		if (pos != -1)
			domain = domain.left(pos);
		domainLen = domain.length();		//域名长度
		afterPointStart = domain.length() - 1;//后点开始
		for (int i = 0; i < pointNum; i++)
			afterPointStart = domain.lastIndexOf(".", afterPointStart);
		frontPointEnd = domain.lastIndexOf(".", afterPointStart);
		if (rem2Domain)
		{
			domain = domain.right(domainLen - frontPointEnd);
		}
		else//保留二级域名
		{
			if (frontPointEnd != -1)//至少是二级域名
			{
				frontPointStart = domain.lastIndexOf(".", frontPointEnd);
				if (frontPointEnd != -1)//至少是三级域名
				{
					domain = domain.right(domainLen - frontPointStart + 1);
				}
			}
		}
	}
	//端口过滤处理--------------------------------------------------------
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

//网页_取端口
//取出url的默认端口
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

//网页_取页面地址
QString nmsl::e_network::getUrlAddr(QString url)
{
	QString urlAddr;	//页面地址
	int pos;			//符号位置
	QString domain;		//域名
	domain = getDomain(url);
	pos = url.indexOf("/", url.indexOf(domain));
	if (pos > 0)
		urlAddr = url.right(url.length() - pos);
	else
		urlAddr = "/";
	return(urlAddr);
}

//网页_Cookie合并更新
//返回更新后的Cookie
QString nmsl::e_network::cookieUpdate(QString& oldCookie, QString newCookie)
{
	QStringList oldCookie_List;
	QStringList newCookie_List;
	int count;
	//初步格式化
	oldCookie.remove(QRegExp("\\s"));//删全部空
	newCookie.remove(QRegExp("\\s"));//删全部空
	//开始更新Cookie
	oldCookie_List = oldCookie.split(";");
	newCookie_List = newCookie.split(";");
	for (int i = 0; i < oldCookie_List.length(); i++)
	{
		if (nmsl::e_inside::stringListIsExistence1(newCookie_List, nmsl::e_inside::cookieGetName(oldCookie_List[i])) == false)
			newCookie_List << oldCookie_List[i];
	}
	//去除无效的Cookie
	oldCookie = "";
	for (int i = 0; i < newCookie_List.length(); i++)
	{
		if (newCookie_List[i].right(8) != "=deleted")
			oldCookie += newCookie_List[i] + "; ";
	}
	oldCookie = oldCookie.left(oldCookie.length() - 2);
	//对重复的Cookie进行合并
	oldCookie = nmsl::e_text::removeRepeatText(oldCookie, "; ");
	return oldCookie;
}



//网页_取单条Cookie
//cookie：全部cookie
//name：要获取的cookie名称
//noName：是否不附带名称，默认附带名称
QString nmsl::e_network::getLineCookie(QString cookie, QString name, bool noName)
{
	QString t;
	cookie = cookie.trimmed();//删首尾空
	if (cookie.right(1) != ";")
		cookie += ";";
	t = nmsl::e_inside::getlinecookie1(cookie, name);
	if (noName)
		return t;
	else
		return name + "=" + t;
}

//网页_取Cookie
//domain：只接受域名
//cookieName：，Cookie名称，为空返回全部Cookie
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
	e_network::cookieUpdate(val, QString(tmp1));//Cookie合并更新
	if (cookieName != "")
	{

	}
	return val;
}

//网页_取外网IP
//pos：所在省市
QString nmsl::e_network::getExtIP(QString& pos)
{
	QString source;
	source = http_access("https://2020.ip138.com/");
	source = nmsl::e_text::getMiddleString(source, "您的iP", "</p>");
	pos = nmsl::e_text::getMiddleString(source, "来自：", "\r\n");
	return nmsl::e_text::getMiddleString(source, "地址是：[", "] 来自：");
}

//网页_打开指定网址
//使用系统默认的浏览器打开一个网址
//url：要打开的网页地址
void nmsl::e_network::openUrl(QString url)
{
	ShellExecuteA(0, "open", url.toUtf8().data(), "", "", 1);
}

//网页_清除Cookie
//domain：只接受域名
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



//网页_访问
//使用WinInet的方式访问网页
QByteArray nmsl::e_network::http_access(
	QString url,					//完整的网页地址,必须包含http://或者https://
	int type,						//0=GET 1=POST 2=HEAD
	QString postData,				//POST专用
	QString& sendCookies,			//提交Cookies
	QString& returnCookies,			//返回的Cookie
	QString headers,				//附加协议头
	QString& returnHeaders,			//返回协议头
	bool disableRedirection,		//禁止重定向,默认不禁止网页重定向
	QByteArray sendByteArrar,		//提交ByteArray数据
	QString privoxy)				//代理地址，格式为 8.8.8.8:88
{
	HINTERNET internetHandle;		//局_Internet句柄
	HINTERNET internetConnectHandle;//局_Internet连接句柄
	HINTERNET httpReqHandle;		//局_HTTP请求句柄
	int count;						//局_计次
	//QByteArray ByteArray_tmp;		//局_字节集
	QString req_type;				//局_访问方式
	QString User_Agent;				//局_User_Agent
	QStringList header_return;		//局_返回协议头
	QByteArray	content;			//局_页面内容
	unsigned int reqSign;			//局_请求标记
	QString redirectionAddr;		//局_重定向地址
	bool isHttps;					//局_HTTPS


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

	if (headers.indexOf("User-Agent:", 0, Qt::CaseInsensitive) != -1)//如果参数附加协议头里有"User-Agent:"
	{
		User_Agent = nmsl::e_text::getMiddleString(headers, "User-Agent:", "\r\n");//把参数附加协议头里的"User-Agent:"给User_Agent
		if (User_Agent == "")
		{
			if (headers.indexOf(":") != -1)
				User_Agent = headers.right(headers.length() - headers.indexOf(":"));
		}
	}
	else
	{
		User_Agent = "Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)";//如果没有就自己指定一个
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

	reqSign = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;//局_请求标记
	if (sendCookies != "")//如果参数中提供了Cookies
		reqSign |= INTERNET_FLAG_NO_COOKIES;

	if (disableRedirection)//如果禁止重定向
		reqSign |= INTERNET_FLAG_NO_AUTO_REDIRECT;

	if (isHttps)//是否https
		reqSign |= INTERNET_FLAG_SECURE;
	else
		reqSign |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;

	//局_HTTP请求句柄
	httpReqHandle = HttpOpenRequestA(internetConnectHandle, req_type.toUtf8().data(), getUrlAddr(url).toUtf8().data(),
		QString("HTTP/1.1").toUtf8().data(), (char)0, (char)0, reqSign, 0);
	if (httpReqHandle == 0)
	{
		InternetCloseHandle(httpReqHandle);
		InternetCloseHandle(internetConnectHandle);
		InternetCloseHandle(internetHandle);
		return QByteArray();
	}

	if (headers == "")//附加协议头
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

	if (sendCookies != "")//提交Cookies
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
	count = 0;//局_计次
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

	if (sendCookies == "")//如果没有提供参数sendCookies
	{
		cookieUpdate(sendCookies, returnCookies);//网页_Cookie合并更新
	}
	return content;
}


