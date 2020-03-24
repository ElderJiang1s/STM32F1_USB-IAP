#ifndef __NMSL_H
#define __NMSL_H
#include "includes.h"
#pragma comment(lib,"wininet.lib")
//2020.2.28


namespace nmsl
{
	//内部
	class e_inside
	{
	public:
		static QString cookieGetName(QString cookie);//内部_Cookie取名
		static QString cookieGetVal(QString cookie);//内部_Cookie取值
		static QString headerGetName(QString header);//内部_协议头取名
		static QString headerGetVal(QString header);//内部_协议头取值
		static bool stringListIsExistence1(QStringList arr, QString val);//内部_数组成员是否存在1
		static int stringListIsExistence_Text(QStringList arr, QString val);//内部_数组成员是否存在_文本
		static QString getlinecookie1(QString cookie, QString name);//内部_取单条Cookie1
	};

	//系统
	class e_system
	{
	public:
		static QByteArray fileLoad(QString name);//系统_读取文件
		static bool fileSave(QString name, QByteArray data, bool isCover = false);//系统_保存文件
		static void getDisplayInf(short& colordepth, short& width, short& height, short& efreshRate);//系统_取屏幕分辨率
		static bool delay_ms(int nms);//系统_延时ms
		static bool isdebug();//程序_是否被调试
		unsigned int cal_crc(unsigned int* ptr, int len);

	};

	//文本
	class e_text
	{
	public:
		static QString getMiddleString(QString text, QString befor, QString after, int start_pos = 0, Qt::CaseSensitivity cs = Qt::CaseInsensitive);//取出中间文本
		static QString removeRepeatText(QString text, QString sep);//文本_去重复文本
		static QString getLeft(QString text, QString find, int start_pos = 0, Qt::CaseSensitivity cs = Qt::CaseInsensitive);//文本_取左边
		static QString getRight(QString text, QString find, int start_pos = -1, Qt::CaseSensitivity cs = Qt::CaseInsensitive);//文本_取左边
	};

	//网页
	class e_network
	{
	public:
		static QString getDomain(QString url, bool rem2Domain = false, bool nofilterPort = false, int pointNum = -1);//网页_取域名
		static int getPort(QString url);//网页_取端口
		static QString getUrlAddr(QString url);//网页_取页面地址
		static QString cookieUpdate(QString& oldCookie, QString newCookie);//网页_Cookie合并更新
		static QString getLineCookie(QString cookie, QString name, bool noName = false);//网页_取单条Cookie
		static QString getCookie(QString domain, QString cookieName = "");//网页_取Cookie
		static QString getExtIP(QString& pos = QString());//网页_取外网IP
		static void openUrl(QString url);//网页_打开指定网址
		static void clearCookie(QString domain);//网页_清除Cookie


		static	QByteArray http_access(					//网页_访问
			QString url,								//完整的网页地址,必须包含http://或者https://
			int type = 0,								//0=GET 1=POST 2=HEAD
			QString postData = "",						//POST专用
			QString& sendCookies = QString(),			//提交Cookies
			QString& returnCookies = QString(),			//返回的Cookie
			QString headers = "",						//附加协议头
			QString& returnHeaders = QString(),			//返回协议头
			bool disableRedirection = false,			//禁止重定向,默认不禁止网页重定向
			QByteArray sendByteArrar = "",				//提交ByteArray数据
			QString privoxy = "");						//代理地址，格式为 8.8.8.8:88
	};


	//键盘
	class e_keyboard
	{
	public:
		static void keyClick(int key, int status = 0);//键盘_单击
		static void keyboardUnlock();//键盘_解锁
		static void keyboardLock(bool status = false);//键盘_锁定
		static void keyboardMessage(long long windowHandle, int key, int status = 1, bool isFkey = false);//键盘_消息
		static void combinationKey(int key, int Fkey1, int Fkey2 = 0, int Fkey3 = 0);//键盘_组合按键




		static const int key_0 = 48; static const int key_1 = 49; static const int key_2 = 50; static const int key_3 = 51;
		static const int key_4 = 52; static const int key_5 = 53; static const int key_6 = 54; static const int key_7 = 55;
		static const int key_8 = 56; static const int key_9 = 57;
		static const int key_A = 65; static const int key_B = 66; static const int key_C = 67; static const int key_D = 68;
		static const int key_E = 69; static const int key_F = 70; static const int key_G = 71; static const int key_H = 72;
		static const int key_I = 73; static const int key_J = 74; static const int key_K = 75; static const int key_L = 76;
		static const int key_M = 77; static const int key_N = 78; static const int key_O = 79; static const int key_P = 80;
		static const int key_Q = 81; static const int key_R = 82; static const int key_S = 83; static const int key_T = 84;
		static const int key_U = 85; static const int key_V = 86; static const int key_W = 87; static const int key_X = 88;
		static const int key_Y = 89; static const int key_Z = 90;
		static const int key_F1 = 112; static const int key_F2 = 113; static const int key_F3 = 114; static const int key_F4 = 115;
		static const int key_F5 = 116; static const int key_F6 = 117; static const int key_F7 = 118; static const int key_F8 = 119;
		static const int key_F9 = 120; static const int key_F10 = 121; static const int key_F11 = 122; static const int key_F12 = 123;
		static const int key_F13 = 124; static const int key_F14 = 125; static const int key_F15 = 126; static const int key_F16 = 127;
		static const int key_Break = 3;			//Break键
		static const int key_BackSpace = 8;		//退格键
		static const int key_Tab = 9;			//Tab键
		static const int key_Enter = 13;		//回车键
		static const int key_Shift = 16;		//Shift键
		static const int key_Ctrl = 17;			//Ctrl键
		static const int key_Alt = 18;			//Alt键
		static const int key_Pause = 19;		//Pause键
		static const int key_CapsLock = 20;		//CapsLock键
		static const int key_NumLock = 144;		//NumLock键
		static const int key_ScrollLock = 145;	//ScrollLock键
		static const int key_Esc = 27;			//Esc键
		static const int key_Space = 32;		//空格键
		static const int key_PageUp = 33;		//PageUp键
		static const int key_PageDown = 34;		//PageDown键
		static const int key_Home = 36;			//Home键
		static const int key_End = 35;			//End键
		static const int key_Ins = 45;			//Ins键
		static const int key_Del = 46;			//Del键

	private:
		static int keyboard_HookProcess(int icode, int wparam, long long lparam);//键盘钩子处理(锁键盘)
	};

	//鼠标
	class e_mouse
	{
	public:
		static void mousePressKey(int key = 1, int type = 1);//鼠标_按键
		static void mouseMove(int x, int y, long long windowHandle = 0);//鼠标_移动
		static tagPOINT mouseGetPos();//鼠标_取位置
		static void mouseShowCursor(bool status);//鼠标_显示隐藏
		static void mouseLock(bool status = false);//鼠标_锁定
		static void mouseUnlock();//鼠标_解锁
		static void mouseLimit(int x, int y, int width, int height);//鼠标_限制
		static void mouseMessage(long long windowHandle, int x, int y, int key = 1, int status = 1);//鼠标_消息
		static void mouseSwapButton(bool isSwap);//鼠标_左右键交换


	private:
		static int mouse_HookProcess(int code, int mouse, long long lp);//鼠标钩子处理(锁鼠标)

	};

	//进程
	class e_process
	{
	public:
		static int processCreate(QString name,QString argv = "");//进程_创建
		static int processOpen(long long windowHandle);//进程_打开
	};

}



#endif
