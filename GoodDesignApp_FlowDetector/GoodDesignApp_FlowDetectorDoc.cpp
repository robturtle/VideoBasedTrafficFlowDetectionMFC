// GoodDesignApp_FlowDetectorDoc.cpp : CFDDoc ���ʵ��
//

#include "stdafx.h"
#include "GoodDesignApp_FlowDetector.h"

#include "GoodDesignApp_FlowDetectorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFDDoc

IMPLEMENT_DYNCREATE(CFDDoc, CDocument)

BEGIN_MESSAGE_MAP(CFDDoc, CDocument)
END_MESSAGE_MAP()


// CFDDoc ����/����

CFDDoc::CFDDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CFDDoc::~CFDDoc()
{
}

BOOL CFDDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CFDDoc ���л�

void CFDDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CFDDoc ���

#ifdef _DEBUG
void CFDDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFDDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFDDoc ����
