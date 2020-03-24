#include <QtWidgets/QApplication>
#include "QDebug.h"				//DEBUG
#include "QMessageBox.h"		//信息框
#include "qpushbutton.h"		//按钮
#include "qtextedit.h"			//编辑框
#include "qmenubar.h"			//菜单栏
#include "qtoolbar.h"			//工具栏
#include "qstatusbar.h"			//状态栏
#include "qlabel.h"				//标签
#include "qdockwidget.h"		//浮动窗口
#include "qdialog.h"			//对话框
#include "qcolordialog.h"		//颜色选择对话框
#include "qevent.h"				//事件
#include "qtimer.h"				//定时器
#include "qpainter.h"			//画家
#include "qimage.h"				//QImage绘图设备
#include "qfiledialog.h"		//文件选择对话框
#include "qfile.h"				//文件读写
#include "qtextcodec.h"			//文本编码
#include "qfileinfo.h"			//文件信息
#include "qserialport.h"		//Serial Port
#include "qserialportinfo.h"	//Serial Port Info
#include "qcryptographichash.h" //加密
#include "qprogressbar.h"		//进度条
#include "qdatetime.h"			//时间


#include "Windows.h"
#include "WinInet.h"


#define cout qDebug()


#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
