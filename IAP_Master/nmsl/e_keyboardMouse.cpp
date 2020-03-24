#include "nmsl.h"

static long long keyboard_lockHookHandle;//�����������Ӿ��
static bool keyboard_status;//��_����״̬��true=��ȫ����(Ĭ��Ϊfalse)��false=�������ܼ���Win��Esc��F4��Tab��Print��Soroll��Pause��
static long long mouse_lockHookHandle;//����������Ӿ��
static bool mouse_status;//�������״̬��true:��ȫ������� (Ĭ��Ϊfalse) false:����������Ҽ�

//���̹��Ӵ���(������)
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

//��깳�Ӵ���(�����)
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


//����_����
//key��������
//status��0 ������1 ���� ��2 �ſ�
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

//����_����
//���̽���(����������,�����ָ����̵�ʹ��)
void nmsl::e_keyboard::keyboardUnlock()
{
	UnhookWindowsHookEx((HHOOK)keyboard_lockHookHandle);
}

//����_����
void nmsl::e_keyboard::keyboardLock(bool status)
{
	keyboard_status = status;
	keyboard_lockHookHandle = (long long)SetWindowsHookExA(13, (HOOKPROC)keyboard_HookProcess, GetModuleHandleA(0), 0);
}

//����_��Ϣ
//��ָ�����ھ���Ĵ����з��Ͱ�����Ϣ(�޷���ֵ)
//windowHandle�����ھ����������Ϣ�Ĵ��ھ��
//key�������룬�����ļ�����
//status��״̬��Ĭ��Ϊ1  1=�����ַ�(��д) 2=�����ַ�(Сд)  3=���£�4=�ſ���5=����
//isFkey���Ƿ��ܼ���Ĭ��Ϊflase����ͨ��   true:���ܼ� (Ϊ���ܼ��������ȼ����ܲ������ַ�)
void nmsl::e_keyboard::keyboardMessage(long long windowHandle, int key, int status, bool isFkey)
{
	int push;//����
	int pop;//�ſ�
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

//����_��ϰ���
//ģ��һ����ϰ�������:Alt+F4
//�����룺��F4
//���ܼ���1����Alt
//���ܼ���2����������
//���ܼ���3����������
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





//���_����
//key������1 ���  2 �Ҽ�  3 �м���Ĭ��Ϊ���
//type��������ʽ��1 ����  2 ˫��  3 ���� 4 �ſ���Ĭ��Ϊ����
void nmsl::e_mouse::mousePressKey(int key, int type)
{
	if (key == 1)//���
	{
		if (type == 3)//����
		{
			mouse_event(2, 0, 0, 0, 0);
		}
		else if (type == 4)//�ſ�
		{
			mouse_event(4, 0, 0, 0, 0);
		}
		else if (type == 1)//����
		{
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
		}
		else if (type == 2)//˫��
		{
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(2, 0, 0, 0, 0);
			mouse_event(4, 0, 0, 0, 0);
		}
	}
	else if (key == 2)//�Ҽ�
	{
		if (type == 3)//����
		{
			mouse_event(8, 0, 0, 0, 0);
		}
		else if (type == 4)//�ſ�
		{
			mouse_event(16, 0, 0, 0, 0);
		}
		else if (type == 1)//����
		{
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
		}
		else if (type == 2)//˫��
		{
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(8, 0, 0, 0, 0);
			mouse_event(16, 0, 0, 0, 0);
		}
	}
	else if (key == 3)//�м�
	{
		if (type == 3)//����
		{
			mouse_event(32, 0, 0, 0, 0);
		}
		else if (type == 4)//�ſ�
		{
			mouse_event(64, 0, 0, 0, 0);
		}
		else if (type == 1)//����
		{
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
		}
		else if (type == 2)//˫��
		{
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
			nmsl::e_system::delay_ms(GetDoubleClickTime());
			mouse_event(32, 0, 0, 0, 0);
			mouse_event(64, 0, 0, 0, 0);
		}
	}
}

//���_�ƶ�
//x��ˮƽ����
//y����ֱ����
//windowHandle�����ھ����Ĭ��Ϊ0����������ƶ����  �����Ч����Դ����������ƶ�
void nmsl::e_mouse::mouseMove(int x, int y, long long windowHandle)
{
	tagPOINT tmp;
	tmp.x = 0;
	tmp.y = 0;
	if (windowHandle != 0)
		ClientToScreen((HWND)windowHandle, &tmp);
	SetCursorPos(tmp.x + x, tmp.y + y);
}

//���_ȡλ��
//ȡ�������Ļ�е�����λ��
tagPOINT nmsl::e_mouse::mouseGetPos()
{
	tagPOINT tmp;
	GetCursorPos(&tmp);
	return tmp;
}

//���_��ʾ����
//�������ָ��Ŀ�����, ֻ�����������������
//status��true ����  false ��ʾ
void nmsl::e_mouse::mouseShowCursor(bool status)
{
	ShowCursor(!status);
}

//���_����
//��������(������,������Ч)
//status��״̬��true:��ȫ������� (Ĭ��Ϊfalse) false:����������Ҽ�
void nmsl::e_mouse::mouseLock(bool status)
{
	mouse_status = status;
	mouse_lockHookHandle = (long long)SetWindowsHookEx(14, (HOOKPROC)mouse_HookProcess, GetModuleHandleA(0), 0);
}

//���_����
void nmsl::e_mouse::mouseUnlock()
{
	UnhookWindowsHookEx((HHOOK)mouse_lockHookHandle);
}

//���_����
//�������Ļ��Χ(�޷���ֵ)
//x�����Χ���Ͻ�x
//y�����Χ���Ͻ�y
//width�����Χ���
//height�����Χ�߶�
void nmsl::e_mouse::mouseLimit(int x, int y, int width, int height)
{
	RECT aa;
	aa.left = x;
	aa.top = y;
	aa.right = x + width;
	aa.bottom = y + height;
	ClipCursor(&aa);
}

//���_��Ϣ
//windowHandle�����ھ��
//x���ƶ�Ŀ�����ˮƽ���� (ע:����Ϊ����1���ھ���е�����)
//y���ƶ�Ŀ����괹ֱ����
//key��Ĭ��Ϊ�����1 ���  2 �Ҽ�  3 �м� 4 �м��Ϲ��� 5 �м��¹���(����ǰ���밴�·ſ��м�)
//statue��Ĭ��Ϊ������1 ����  2 ˫��  3 ���� 4 �ſ�
void nmsl::e_mouse::mouseMessage(long long windowHandle, int x, int y, int key, int status)
{
	long long pos_val;

	pos_val = x + y * 65536;
	if (key == 4)//�м��Ϲ���
	{
		PostMessageA((HWND)windowHandle, 522, 7864336, pos_val + 25100873);
		return;
	}
	if (key == 5)//�м��¹���
	{
		PostMessageA((HWND)windowHandle, 522, -7864304, pos_val + 25100873);
		return;
	}
	PostMessageA((HWND)windowHandle, 512, 2, pos_val);
	if (key == 1)//���
	{
		if (status == 1)//����
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
		}
		if (status == 2)//˫��
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
			PostMessageA((HWND)windowHandle, 515, 0, pos_val);
		}
		if (status == 3)//����
		{
			PostMessageA((HWND)windowHandle, 513, 1, pos_val);
		}
		if (status == 4)//�ſ�
		{
			PostMessageA((HWND)windowHandle, 514, 0, pos_val);
		}
		return;
	}
	if (key == 2)
	{
		if (status == 1)//����
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
		}
		if (status == 2)//˫��
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
			PostMessageA((HWND)windowHandle, 518, 0, pos_val);
		}
		if (status == 3)//����
		{
			PostMessageA((HWND)windowHandle, 516, 2, pos_val);
		}
		if (status == 4)//�ſ�
		{
			PostMessageA((HWND)windowHandle, 517, 2, pos_val);
		}
		PostMessageA((HWND)windowHandle, 123, 132782, pos_val + 25100873);//�Ҽ��ſ�
		return;
	}
	if (key == 3)
	{
		if (status == 1)//����
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
		}
		if (status == 2)//˫��
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
			PostMessageA((HWND)windowHandle, 521, 0, pos_val);
		}
		if (status == 3)//����
		{
			PostMessageA((HWND)windowHandle, 519, 16, pos_val);
		}
		if (status == 4)//�ſ�
		{
			PostMessageA((HWND)windowHandle, 520, 0, pos_val);
		}
		return;
	}

}

//���_���Ҽ�����
//isSwap��true ���Ҽ�������flase �ָ�
void nmsl::e_mouse::mouseSwapButton(bool isSwap)
{
	SwapMouseButton(isSwap);
}


