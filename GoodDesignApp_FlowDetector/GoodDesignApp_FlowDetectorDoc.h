// GoodDesignApp_FlowDetectorDoc.h : CFDDoc ��Ľӿ�
//


#pragma once


class CFDDoc : public CDocument
{
protected: // �������л�����
	CFDDoc();
	DECLARE_DYNCREATE(CFDDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CFDDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


