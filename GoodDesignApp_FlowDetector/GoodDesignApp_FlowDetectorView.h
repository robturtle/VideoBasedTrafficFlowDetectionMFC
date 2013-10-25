// GoodDesignApp_FlowDetectorView.h : CFDView ��Ľӿ�
//
#pragma once
#include "avi.h"
#include "coordinate_convert.h"
#include "DetectorStrainer.h"
#include<vector>


class CFDView : public CView
{
protected: // �������л�����
	CFDView();
	DECLARE_DYNCREATE(CFDView)

// ����
public:
	CFDDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CFDView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	//��Ƶ����
	Cavi m_avi;		// Cavi �����ڿ�����Ƶ���ݻ�ȡ�򲥷�
	//����ѡ
	CPoint m_ptOrigin;	// ��������ѡ�ĵ�һ��
	vector<CRect> m_rcZoneRecord;		//�����ѡ��ȫ��¼��ָ�����ע�⹹��������
	bool m_bZoneSelecting;			//�����Ƿ񼤻��ѡ����
	//������
	vector<c_detector_strainer> m_lstDetectorStrainer;//������ɸѡ��
	vector<c_flow_counter> m_lstFlowCounter;		//ÿ��һ������������
	//����
	bool m_bShowCurIntense;
	bool m_bShowSumIntense;
	bool m_bFlowDetectingOn;
	void detector_reflush(void);
#ifdef _DATA_COLLECT
	//�����ݲɼ��й�
	int m_nPointStatus;
#endif

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	//����Ƶ���벥���й�
	afx_msg void OnFileOpen();		//��AVI�ļ�
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//������ѡ�����й�
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnZoneSelect();
	afx_msg void OnShowCurTop();
	afx_msg void OnShowSumTop();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnWriteData();
};

#ifndef _DEBUG  // GoodDesignApp_FlowDetectorView.cpp �еĵ��԰汾
inline CFDDoc* CFDView::GetDocument() const
   { return reinterpret_cast<CFDDoc*>(m_pDocument); }
#endif

