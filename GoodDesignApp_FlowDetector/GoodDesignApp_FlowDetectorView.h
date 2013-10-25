// GoodDesignApp_FlowDetectorView.h : CFDView 类的接口
//
#pragma once
#include "avi.h"
#include "coordinate_convert.h"
#include "DetectorStrainer.h"
#include<vector>


class CFDView : public CView
{
protected: // 仅从序列化创建
	CFDView();
	DECLARE_DYNCREATE(CFDView)

// 属性
public:
	CFDDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CFDView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	//视频播放
	Cavi m_avi;		// Cavi 类用于控制视频数据获取或播放
	//鼠标框选
	CPoint m_ptOrigin;	// 保存鼠标框选的第一点
	vector<CRect> m_rcZoneRecord;		//保存框选的全记录（指针变量注意构造析构）
	bool m_bZoneSelecting;			//决定是否激活框选操作
	//检测操作
	vector<c_detector_strainer> m_lstDetectorStrainer;//保存检测筛选器
	vector<c_flow_counter> m_lstFlowCounter;		//每组一个流量计数器
	//控制
	bool m_bShowCurIntense;
	bool m_bShowSumIntense;
	bool m_bFlowDetectingOn;
	void detector_reflush(void);
#ifdef _DATA_COLLECT
	//与数据采集有关
	int m_nPointStatus;
#endif

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	//与视频打开与播放有关
	afx_msg void OnFileOpen();		//打开AVI文件
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//与鼠标框选操作有关
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnZoneSelect();
	afx_msg void OnShowCurTop();
	afx_msg void OnShowSumTop();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnWriteData();
};

#ifndef _DEBUG  // GoodDesignApp_FlowDetectorView.cpp 中的调试版本
inline CFDDoc* CFDView::GetDocument() const
   { return reinterpret_cast<CFDDoc*>(m_pDocument); }
#endif

