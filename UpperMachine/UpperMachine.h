
// UpperMachine.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUpperMachineApp:
// �йش����ʵ�֣������ UpperMachine.cpp
//

class CUpperMachineApp : public CWinApp
{
public:
	CUpperMachineApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUpperMachineApp theApp;