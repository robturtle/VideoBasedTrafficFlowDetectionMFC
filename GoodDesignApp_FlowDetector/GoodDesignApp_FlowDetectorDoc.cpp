// GoodDesignApp_FlowDetectorDoc.cpp : CFDDoc 类的实现
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


// CFDDoc 构造/析构

CFDDoc::CFDDoc()
{
	// TODO: 在此添加一次性构造代码

}

CFDDoc::~CFDDoc()
{
}

BOOL CFDDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CFDDoc 序列化

void CFDDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CFDDoc 诊断

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


// CFDDoc 命令
