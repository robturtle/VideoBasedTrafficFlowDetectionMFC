// GoodDesignApp_FlowDetectorView.cpp : CFDView 类的实现
//

#include "stdafx.h"
#include "GoodDesignApp_FlowDetector.h"

#include "GoodDesignApp_FlowDetectorDoc.h"
#include "GoodDesignApp_FlowDetectorView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFDView

IMPLEMENT_DYNCREATE(CFDView, CView)

BEGIN_MESSAGE_MAP(CFDView, CView)
	ON_COMMAND(ID_FILE_OPEN, &CFDView::OnFileOpen)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDM_ZONE_SELECT, &CFDView::OnZoneSelect)
	ON_COMMAND(IDM_SHOW_CUR_TOP, &CFDView::OnShowCurTop)
	ON_COMMAND(IDM_SHOW_SUM_TOP, &CFDView::OnShowSumTop)
	ON_WM_KEYDOWN()
	ON_COMMAND(IDM_WRITE_DATA, &CFDView::OnWriteData)
END_MESSAGE_MAP()

// CFDView 构造/析构

CFDView::CFDView()
:m_ptOrigin(0,0)
,m_bZoneSelecting(false)
,m_bShowCurIntense(false)
,m_bShowSumIntense(true)
,m_bFlowDetectingOn(true)
{
#ifdef _DATA_COLLECT
	m_nPointStatus = 0;
#endif
}

CFDView::~CFDView()
{
}

BOOL CFDView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CFDView 绘制

void CFDView::OnDraw(CDC* /*pDC*/)
{
	CFDDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//资源申请，初始化
	CDC *pDC=GetDC();			//手动申请CDC，务必记得回收！
	CPen penGreen(0, 2, RGB(0,255,0));
	CPen penRed(0, 2, RGB(255,0,0));
	CPen penNull(PS_NULL, 0, RGB(0,0,0));
	CFont font;
	font.CreatePointFont(450,"微软雅黑");
	CBrush *pBrushNull=CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush BrushWhite(RGB(255,255,255));
	CBrush BrushGreen(RGB(0,255,0));
	CBrush BrushYellow(RGB(255,255,0));
	CBrush BrushRed(RGB(255,0,0));
	CBrush BrushPurple(RGB(255,0,255));
	COLORREF clrGreen=RGB(0,255,0), clrYellow=RGB(255,255,0);
	//设置字体颜色和大小
	pDC->SetTextColor(clrGreen);
	pDC->SelectObject(font);
	//设置画笔和画布
	pDC->SelectObject(penGreen);
	pDC->SelectObject(pBrushNull);

	//变量初始化
	CRect rcClient;//客户区矩形
	GetClientRect(rcClient);
	int nBmpWidth=m_avi.GetWidth(),
		nBmpHeight=m_avi.GetHeight();

	// 按键OnFileOpen打开AVI文件，OnTimer控制视频播放，OnDraw实现视频播放，OnEraseBk消除闪烁现象
	if(m_avi.IsValid()){
		pDC->SetBkMode(TRANSPARENT);//将背景模式设置为透明！！！！！未来将此处移到输出文字处
		int frame = m_avi.GetCurrentFrame();
		CString strFrame; strFrame.Format("frame: %d", frame);
		m_avi.ShowFrame(pDC,rcClient);
		//pDC->TextOut(0, 0, strFrame, strlen(strFrame));
	}

	//鼠标框选操作处理
	int nSize=m_rcZoneRecord.size();
	for(int i=0; i<nSize; i++){
		CRect rcZone=m_rcZoneRecord.at(i);
		//pDC->Rectangle(rcZone);

		//填充BMP测试
		CRect rcBmp=Ccor_convert::rcClientToBmp(rcZone, rcClient, nBmpWidth, nBmpHeight);
		/*CString str; str.Format("%d,%d,%d,%d",rcBmp.left,rcBmp.top,rcBmp.right,rcBmp.bottom);
		pDC->TextOut(0, 0, str);*/
		/*if(m_avi.IsValid())
			m_avi.FillRect(rcBmp);*/
		/*if(m_tmp.is_valid())
			m_tmp.get_intense();*/
	}

	//筛选器工作
	int nSizeDS=m_lstDetectorStrainer.size();
	pDC->SetBkMode(TRANSPARENT);
	for(int i=0; i<nSizeDS; i++){
		CRect rcZone=m_rcZoneRecord.at(i);
		c_detector_strainer &thisDS = m_lstDetectorStrainer.at(i);
		if(!thisDS.is_flush_finished()){
			thisDS.statistic();
			//pDC->TextOut(rcZone.right, rcZone.top, "刷新中...");
		} else {
			//pDC->TextOut(rcZone.right, rcZone.top, "刷新完毕");
		}

		c_flow_counter &thisFC = m_lstFlowCounter.at(i);
		switch(thisFC.is_occupied(thisDS)){
			case RET_OCCUPIED:
				//pDC->TextOut(rcZone.left, rcZone.bottom, "占用");
				break;
			case RET_NO_OCCUPIED:
				//pDC->TextOut(rcZone.left, rcZone.bottom, "未占用");
				break;
			case RET_INITIALIZING:
				//pDC->TextOut(rcZone.left, rcZone.bottom, "初始化中...");
				break;
		}
#ifdef _DATA_COLLECT
		thisFC.write_data(thisDS, "test.csv", m_nPointStatus);
#endif
		int flow = thisFC.get_flow_sum();
		CString str; str.Format("%d",flow);
		pDC->TextOut(rcZone.left, rcZone.bottom+30, str);


		//画区域
		pDC->SelectObject(penNull);
		CRect *ptops = NULL; 
		CRect rc_tofill;
		nSize = thisDS.get_que_max();
#define fill_rect(p,brush)	{\
	Ccor_convert::rcBmpToClient(rcClient, *(p), m_avi.GetWidth(), m_avi.GetHeight(), rc_tofill);\
	pDC->SelectObject(brush);\
	pDC->Rectangle(rc_tofill);}
		if(m_bShowCurIntense){
			thisDS.get_cur_top_rect(ptops);
			for(int j=0; j<nSize; j++) fill_rect(ptops+j,BrushGreen);
			thisDS.get_cur_bottom_rect(ptops);
			for(int j=0; j<nSize; j++) fill_rect(ptops+j,BrushYellow);
		}
		if(m_bShowSumIntense){
			thisDS.get_sum_top_rect(ptops);
			for(int j=0; j<nSize; j++){
				CRect ptmp = ptops + j;
				if(NULL == ptmp) break;
				fill_rect(ptmp,BrushRed);
			}
			thisDS.get_sum_bottom_rect(ptops);
			for(int j=0; j<nSize; j++){
				CRect ptmp = ptops + j;
				if(NULL == ptmp) break;
				fill_rect(ptmp,BrushPurple);
			}
		}
	}

	//资源释放
	ReleaseDC(pDC);			//回收CDC
}


// CFDView 诊断

#ifdef _DEBUG
void CFDView::AssertValid() const
{
	CView::AssertValid();
}

void CFDView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFDDoc* CFDView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFDDoc)));
	return (CFDDoc*)m_pDocument;
}
#endif //_DEBUG



// *******************************************AVI文件播放相关************************************************************************
#define EVENT_AVI				1
#define EVENT_DETECTOR_FLUSH	2
#define T_FLUSH_INTERVAL			600000000		//刷新间隔	
// 按键OnFileOpen打开AVI文件，OnTimer控制视频播放，OnDraw实现视频播放，OnEraseBk消除闪烁现象
void CFDView::OnFileOpen()
{
	CFileDialog FileDlg(TRUE);
	FileDlg.m_ofn.lpstrTitle="请选择一个AVI文件";
	FileDlg.m_ofn.lpstrFilter="AVI文件(*.avi)\0*.avi\0所有文件(*.*)\0*.*\0\0";
	if(IDOK==FileDlg.DoModal())
	{
		m_avi.CloseAvi();
		if(!m_avi.OpenAviFile(FileDlg.GetPathName()))
		{
			AfxMessageBox("读取文件时出错");
			return;
		}
		else
		{
			SetTimer(EVENT_AVI, m_avi.GetElapse(), NULL);
			Invalidate();
		}
	}
}

void CFDView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent){
	case EVENT_AVI:
		if(m_avi.IsHalt()) break;
		if(m_avi.OpenNextFrame()){
			Invalidate();
		}
		else{
			m_avi.SetCurrentFrame(0);
			Invalidate();
		}
		break;
	case EVENT_DETECTOR_FLUSH:
		detector_reflush();
		break;
	default:
		break;
	}

	CView::OnTimer(nIDEvent);
}

BOOL CFDView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_avi.IsValid())
		return true;
	else
		return CView::OnEraseBkgnd(pDC);
}



//********************************************************鼠标框选有关*******************************************************************

// LButtonDown和LButtonUp 捕捉框选矩形并处理，OnZoneSelect 激活/关闭开关
void CFDView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bZoneSelecting) return;
	m_ptOrigin=point;

	CView::OnLButtonDown(nFlags, point);
}

void CFDView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//控制
	if(!m_bZoneSelecting) return;
	if(point == m_ptOrigin) return;

	//屏幕画图操作
	CRect rcZone(m_ptOrigin,point);

	//流量检测模块 //ongoing添加新检测区到容器并set_detect_zone
	CRect rc_client; GetClientRect(rc_client);
	CRect rc_zone=Ccor_convert::rcClientToBmp(rcZone, rc_client, m_avi.GetWidth(), m_avi.GetHeight());

	c_detector_strainer newone(m_avi.GetWidth(), m_avi.GetHeight(), m_avi.GetBytesPerLine(), m_avi.GetData());
	if(!newone.set_detect_zone(rc_zone))
		AfxMessageBox("区域过小！");
	else{
		m_lstDetectorStrainer.push_back(newone);
		c_flow_counter newcounter;
		m_lstFlowCounter.push_back(newcounter);
		m_rcZoneRecord.push_back(rcZone);
		SetTimer(EVENT_DETECTOR_FLUSH, T_FLUSH_INTERVAL, NULL);
		Invalidate();
	}

	CView::OnLButtonUp(nFlags, point);
}

void CFDView::OnZoneSelect()
{
	if(!m_avi.IsValid()) return;//仅当已打开视频时才生效

	m_bZoneSelecting=!m_bZoneSelecting;//切换状态

	//打个勾，换个鼠标图标
	CMainFrame *pMain=(CMainFrame*)GetParent();
	if(m_bZoneSelecting){
		pMain->GetMenu()->GetSubMenu(1)->CheckMenuItem(IDM_ZONE_SELECT,MF_BYCOMMAND|MF_CHECKED);//打钩
		SetClassLong(m_hWnd,GCL_HCURSOR,(long)LoadCursor(NULL,IDC_CROSS));//换成十字形
	} else {
		pMain->GetMenu()->GetSubMenu(1)->CheckMenuItem(IDM_ZONE_SELECT,MF_BYCOMMAND&(~MF_CHECKED));
		SetClassLong(m_hWnd,GCL_HCURSOR,(long)LoadCursor(NULL,IDC_ARROW));
	}
	Invalidate();
}



//***************************************************控制函数*********************************************************************
void CFDView::detector_reflush()
{
	int nSize = m_lstDetectorStrainer.size();
	for(int i=0; i<nSize; i++){
		m_lstDetectorStrainer.at(i).switching_flush_status();
	}
}

void CFDView::OnShowCurTop()
{
	if(!m_avi.IsValid()) return;//仅当已打开视频时才生效

	m_bShowCurIntense=!m_bShowCurIntense;//切换状态

	//打个勾
	CMainFrame *pMain=(CMainFrame*)GetParent();
	if(m_bShowCurIntense){
		pMain->GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SHOW_CUR_TOP,MF_BYCOMMAND|MF_CHECKED);//打钩
	} else {
		pMain->GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SHOW_CUR_TOP,MF_BYCOMMAND&(~MF_CHECKED));
	}
	Invalidate();
}

void CFDView::OnShowSumTop()
{
	if(!m_avi.IsValid()) return;//仅当已打开视频时才生效

	m_bShowSumIntense=!m_bShowSumIntense;//切换状态

	//打个勾
	CMainFrame *pMain=(CMainFrame*)GetParent();
	if(m_bShowSumIntense){
		pMain->GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SHOW_SUM_TOP,MF_BYCOMMAND|MF_CHECKED);//打钩
	} else {
		pMain->GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SHOW_SUM_TOP,MF_BYCOMMAND&(~MF_CHECKED));
	}
	Invalidate();
}

//************************************************键盘操作***************************************************************
void CFDView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar){
		case ' ':
			m_avi.SwitchHaltState();
			break;
		case VK_LEFT:
			m_avi.OpenPreFrame();
			Invalidate();
			break;
		case VK_RIGHT:
			m_avi.OpenNextFrame();
			Invalidate();
			break;
		case VK_UP:
			m_avi.SetCurrentFrame(m_avi.GetCurrentFrame() + m_avi.GetJumpStep());
			Invalidate();
			break;
		case VK_DOWN:
			m_avi.SetCurrentFrame(m_avi.GetCurrentFrame() - m_avi.GetJumpStep());
			Invalidate();
			break;
#ifdef _DATA_COLLECT
		case 'R':
			if(m_lstFlowCounter.size()){
				m_lstFlowCounter.at(0).set_collect(true);
			}
			Invalidate();
			break;
		case '1':
			if(m_lstFlowCounter.size()){
				m_lstFlowCounter.at(0).set_collect(true);
			}
			m_nPointStatus = STATUS_NORMAL;
			m_avi.OpenNextFrame();
			Invalidate();
			break;
		case '2':
			if(m_lstFlowCounter.size()){
				m_lstFlowCounter.at(0).set_collect(true);
			}
			m_nPointStatus = STATUS_CAR_PASSINGBY;
			m_avi.OpenNextFrame();
			Invalidate();
			break;
		case '3':
			if(m_lstFlowCounter.size()){
				m_lstFlowCounter.at(0).set_collect(true);
			}
			m_nPointStatus = STATUS_SHADOW;
			m_avi.OpenNextFrame();
			Invalidate();
			break;
#endif
		default:
			break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFDView::OnWriteData()
{
#ifdef _DATA_COLLECT
	if(m_lstFlowCounter.size()){
		m_lstFlowCounter.at(0).set_collect(true);
	}
	Invalidate();
#endif
}
