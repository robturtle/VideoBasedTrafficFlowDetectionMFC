// GoodDesignApp_FlowDetector.h : GoodDesignApp_FlowDetector Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFDApp:
// �йش����ʵ�֣������ GoodDesignApp_FlowDetector.cpp
//

class CFDApp : public CWinApp
{
public:
	CFDApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFDApp theApp;