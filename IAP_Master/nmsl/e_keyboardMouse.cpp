#include "nmsl.h"

static long long keyboard_lockHookHandle;//键盘锁定钩子句柄
static bool keyboard_status;//集_键盘状态，true=锁全键盘(默认为false)，false=锁定功能键与Win键Esc键F4键Tab键Print键Soroll键Pause键
static long long mouse_lockHookHandle;//鼠标锁定钩子句柄
static bool mouse_status;//鼠标锁定状态，true:完全锁定鼠标 (默认为false) false:锁定鼠标左右键

//键盘钩子处理(锁键盘)
int nmsl::e_keyboard::keyboard_HookProcess(int icode, int wparam, long long lparam)
{
	MSG keyInf;
	if (icode == 0)
	{
		if (keyboard_status == false)
		{
			CopyMemory((void*)&keyInf, (void*)lparam, 20);
			if ((keyInf.hwnd == (HWND)key_Esc) || (keyInf.hwnd == (HWND)key_Tab) || (keyInf.hwnd == (HWND)key_F4))
				return 1;
			if ((keyInf.hwnd == (HWND)44) || (keyInf.hwnd == (HWND)145) || (keyInf.hwnd == (HWND)19))
				return 1;
			if ((keyInf.hwnd == (HWND)91) || (keyInf.hwnd == (HWND)92) || (keyInf.hwnd == (HWND)93))
				return 1;
			if ((keyInf.hwnd == (HWND)162) || (keyInf.hwnd == (HWND)163))
				return 1;
			if ((keyInf.hwnd == (HWND)164) || (keyInf.hwnd == (HWND)165))
				return 1;
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return CallNextHookEx((HHOOK)keyboard_lockHookHandle, icode, wparam, lparam);
}

//鼠标钩子处理(锁鼠标)
int nmsl::e_mouse::mouse_HookProcess(int code, int mouse, long long lp)
{
	if (mouse_status)
		return 1;
	if ((mouse == 513) || (mouse == 514) || (mouse == 515))
		return 1;
	if ((mouse == 516) || (mouse == 517) || (mouse == 518))
		return 1;
	return((HHOOK)mouse_lockHookHandle, code, mouse, lp);
}


//键盘_单击
//key：键代码
//status：0 单击，1 按下 ，2 放开
void nmsl::e_keyboard::keyClick(int key, int status)
{
	if (status == 1)
	{
		keybd_event(key, 0, 0, 0);
	}
	else
	{
		if (status == 2)
		{
			keybd_event(key, 0, 2, 0);
		}
		else
		{
			keybd_event(key, 0, 0, 0);
			QApplication::processEvents();
			keybd_event(key, 0, 2, 0);
		}
	}
}

//键盘_解锁
//键盘解锁(键盘锁定后,用来恢复键盘的使用)
void nmsl::e_keyboard::keyboardUnlock()
{
	UnhookWindowsHookEx((HHOOK)keyboard_lockHookHandle);
}

//键盘_锁定
void nmsl::e_keyboard::keyboardLock(bool status)
{
	keyboard_status = status;
	keyboard_lockHookHandle = (long long)SetWindowsHookExA(13, (HOOKPROC)keyboard_HookProcess, GetModuleHandleA(0), 0);
}

//键盘_消息
//向指定窗口句柄的窗口中发送按键消息(无返回值)
//windowHandle：窗口句柄，接收消息的窗口句柄
//key：键代码，按键的键代码
//status：状态，默认为1  1=输入字符(大写) 2=输入字符(小写)  3=按下，4=放开，5=单击
//isFkey：是否功能键，默认为flase：普通键   true:功能键 (为功能键可用于热键技能不输入字符)
void nmsl::e_keyboard::keyboardMessage(long long windowHandle, int key, int status, bool isFkey)
{
	int push;//按下
	int pop;//放开
	if (isFkey)
	{
		push = 260;
		pop = 261;
	}
	else
	{
		push = 256;
		pop = 257;
	}
	switch (status)
	{
	case 1: PostMessageA((HWND)windowHandle, 258, key, 0); break;
	case 2: PostMessageA((HWND)windowHandle, push, key, 0); break;
	case 3: PostMessageA((HWND)windowHandle, push, key, 0); break;
	case 4: PostMessageA((HWND)windowHandle, pop, key, 0); break;
	case 5: PostMessageA((HWND)windowHandle, push, key, 0); break;
	default: break;
	}

}

//键盘_组合按键
//模拟一个组合按键；如:Alt+F4
//键代码：如F4
//功能键码1：如Alt
//功能键码2：可以留空
//功能键码3：可以留空
void nmsl::e_keyboard::combinationKey(int key, int Fkey1, int Fkey2, int Fkey3)
{
	if (Fkey3 != 0)
		keybd_event(Fkey3, 0, 0, 0);
	if (Fkey2 != 0)
		keybd_event(Fkey2, 0, 0, 0);
	keybd_event(Fkey1, 0, 0, 0);
	keybd_event(key, 0, 0, 0);
	if (Fkey3 != 0)
		keybd_event(Fkey3, 0, 2, 0);
	if (Fkey2 != 0)
		keybd_event(Fkey2, 0, 2, 0);
	keybd_event(Fkey1, 0, 2, 0);
	keybd_event(key, 0, 2, 0);
}





//鼠标_按键
//key：键，1 左键  2 右键  3 中键，默认为左键
//type：按键方式，1 单击  2 双击  3 按下 4 放开，默认为单击
void nmsl::e_mouse::mousePressKey(int key, int type)
{
	if (key == 1)//左键
	{
		if (type == 3)//按下
		{
			mouse_event(2, 0, 0, 0, 0);
		}
		else if (type == 4)//放开
		{
			mouse_event(4, 0, 0, 0, 0);
		}
		else if (type == 1)//单击
		{
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
		}
		else if (type == 2)//双击
		{
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
		}
	}
	else if (key == 2)//右键
	{
		if (type == 3)//按下
		{
			mouse_event(8, 0, 0, 0, 0);
		}
		else if (type == 4)//放开
		{
			mouse_event(16, 0, 0, 0, 0);
		}
		else if (type == 1)//单击
		{
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
		}
		else if (type == 2)//双击
		{
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
		}
	}
	else if (key == 3)//中键
	{
		if (type == 3)//按下
		{
			mouse_event(32, 0, 0, 0, 0);
		}
		else if (type == 4)//放开
		{
			mouse_event(64, 0, 0, 0, 0);
		}
		else if (type == 1)//单击
		{
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
		}
		else if (type == 2)//双击
		{
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
		}
	}
}

//鼠标_移动
//x：水平坐标
//y：垂直坐标
//windowHandle：窗口句柄，默认为0：相对桌面移动鼠标  句柄有效则相对窗口中坐标移动
void nmsl::e_mouse::mouseMove(int x, int y, long long windowHandle)
{
	tagPOINT tmp;
	tmp.x = 0;
	tmp.y = 0;
	if (windowHandle != 0)
		ClientToScreen((HWND)windowHandle, &tmp);
	SetCursorPos(tmp.x + x, tmp.y + y);
}

//鼠标_取位置
//取鼠标在屏幕中的坐标位置
tagPOINT nmsl::e_mouse::mouseGetPos()
{
	tagPOINT tmp;
	GetCursorPos(&tmp);
	return tmp;
}

//鼠标_显示隐藏
//设置鼠标指针的可视性, 只能在自身窗口隐藏鼠标
//status：true 隐藏  false 显示
void nmsl::e_mouse::mouseShowCursor(bool status)
{
	ShowCursor(!status);
}

//鼠标_锁定
//锁定鼠标键(锁定后,鼠标键无效)
//status：状态，true:完全锁定鼠标 (默认为false) false:锁定鼠标左右键
void nmsl::e_mouse::mouseLock(bool status)
{
	mouse_status = status;
	mouse_lockHookHandle = (long long)SetWindowsHookEx(14, (HOOKPROC)mouse_HookProcess, GetModuleHandleA(0), 0);
}

//鼠标_解锁
void nmsl::e_mouse::mouseUnlock()
{
	UnhookWindowsHookEx((HHOOK)mouse_lockHookHandle);
}

//鼠标_限制
//限制鼠标的活动范围(无返回值)
//x：活动范围左上角x
//y：活动范围左上角y
//width：活动范围宽度
//height：活动范围高度
void nmsl::e_mouse::mouseLimit(int x, int y, int width, int height)
{
	RECT aa;
	aa.left = x;
	aa.top = y;
	aa.right = x + width;
	aa.bottom = y + height;
	ClipCursor(&aa);
}

//鼠标_消息
//windowHandle：窗口句柄
//x：移动目标鼠标水平坐标 (注:坐标为参数1窗口句柄中的坐标)
//y：移动目标鼠标垂直坐标
//key：默认为左键，1 左键  2 右键  3 中键 4 中键上滚动 5 中键下滚动(滚动前后请按下放开中键)
//statue：默认为单击，1 单击  2 双击  3 按下 4 放开
void nmsl::e_mouse::mouseMessage(long long windowHandle, int x, int y, int key, int status)
{
	long long pos_val;

	pos_val = x + y * 65536;
	if (key == 4)//中键上滚动
	{
		PostMessageA((HWND)windowHandle, 522, 7864336, pos_val + 25100873);
		return;
	}
	if (key == 5)//中键下滚动
	{
		PostMessageA((HWND)windowHandle, 522, -7864304, pos_val + 25100873);
		return;
	}
	PostMessageA((HWND)windowHandle, 512, 2, pos_val);
	if (key == 1)//左键
	{
		if (status == 1)//单击
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
		}
		if (status == 2)//双击
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
			PostMessageA((HWND)windowHandle, 515, 0, pos_val);
		}
		if (status == 3)//按下
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
		}
		if (status == 4)//放开
		{
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
		}
		return;
	}
	if (key == 2)
	{
		if (status == 1)//单击
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
		}
		if (status == 2)//双击
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
			PostMessageA((HWND)windowHandle, 518, 0, pos_val);
		}
		if (status == 3)//按下
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
		}
		if (status == 4)//放开
		{
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
		}
		PostMessageA((HWND)windowHandle, 123, 132782, pos_val + 25100873);//右键放开
		return;
	}
	if (key == 3)
	{
		if (status == 1)//单击
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
		}
		if (status == 2)//双击
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
			PostMessageA((HWND)windowHandle, 521, 0, pos_val);
		}
		if (status == 3)//按下
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
		}
		if (status == 4)//放开
		{
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
		}
		return;
	}

}

//鼠标_左右键交换
//isSwap：true 左右键交换；flase 恢复
void nmsl::e_mouse::mouseSwapButton(bool isSwap)
{
	SwapMouseButton(isSwap);
}


