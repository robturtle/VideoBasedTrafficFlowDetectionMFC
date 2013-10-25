//modified #1
/**********************************************************************************************
 * Last modified by Liu Yang
 * File name:		avi.cpp
 * Platform:		Microsoft MFC
 * Description:		read an avi file and display a specific frame to the given DC
 * Version:		1.3.1
 * Lisence:		GPL v2.0
 * History:
 *				2012.05.14: (v0.1.2) finished basic features;
 *				2012.08.08: (v0.1.3)
 *						rewrite function FillRect to fascilate its usage 
 *						change more functions into inlined
 *				2012.08.13:(v0.1.3.1)
 *						bug repaired
 **********************************************************************************************/
#pragma once
#if !defined(AFX_AVI_H__4BEE9AA3_0FAD_474D_B816_EB3F33476034__INCLUDED_)
#define AFX_AVI_H__4BEE9AA3_0FAD_474D_B816_EB3F33476034__INCLUDED_


#include <Vfw.h>
#pragma comment(lib,"Vfw32.lib")
class Cavi  
{
public:
	//属性
	inline bool IsValid() const{return m_bOpenState;}
	inline UINT GetElapse() const{return m_nElapse;}
	inline long GetFrameLength() const{return m_lFrameLength;}
	inline long GetCurrentFrame() const{return m_nCurrentFrame;}
	inline BYTE*& GetData(){return pData;}
	inline int GetWidth() const{return m_Width;}
	inline int GetHeight() const{return m_Height;}
	inline int GetBytesPerLine() const{return m_BytesPerLine;}
	inline UINT GetJumpStep(void) const{return m_nJumpStep;}
	inline bool IsHalt() const {return m_bHalt;}
	inline void SwitchHaltState() {m_bHalt = !m_bHalt;}

	//操作
	bool OpenAviFile(LPCTSTR lpszPathName);		// 打开AVI文件
	bool CloseAvi();							// 关闭avi
	inline bool OpenFrame(int FrameNumber);		// 打开某帧
	inline bool OpenNextFrame();
	inline bool OpenPreFrame();
	inline void ShowFrame(CDC* pDC,CRect rectShow);	//显示某帧
	inline void SetElapse(UINT newElapse){m_nElapse=newElapse;}	//设置帧间间隔，单位：ms
	inline bool SetCurrentFrame(long newFrameIndex);				//设置当前帧
	//friend class CaviPlayer;						//提供avi播放的操作类(未考虑实现)

	//测试
	void FillRect(CRect rcZone, COLORREF color=RGB(0,255,0));		//填满一个矩形区域，坐标：BMP图像默认坐标，单位：像素

	//实现
	Cavi();    
	virtual ~Cavi(); 
private:
	PAVIFILE	m_pavifile;					// 指向AVI文件指针
	AVIFILEINFO m_pavifileinfo;				// 指向avi文件信息
	PAVISTREAM	m_pavistream;				// 指向avi流
	LPBITMAPINFOHEADER m_lpbmpinfohead;	// 指向位图头信息
	PGETFRAME m_pgetframe; //
	BYTE *pData;				// 象素数据的开头指针
	int m_Width;				// 象素宽
	int m_Height;				// 象素高
	int m_BytesPerLine;		//因为每一行末尾可能有冗余数据，所以每行byte数>=像素宽*
	long m_lFrameLength;		//帧总数
	bool m_bOpenState;		//打开的状态
	//以下是与播放相关的参数
	bool m_bHalt;
	UINT m_nElapse;			//帧间间隔，单位毫秒
	long m_nCurrentFrame;		//当前帧号
	UINT m_nJumpStep;		//每步快进帧数
};

//注意：此函数现在并没有检查视频打开的状态，请注意务必在成功打开AVI文件后再调用此函数
// 内联函数，不得出现判断或循环语句！
inline bool Cavi::OpenFrame(int FrameNumber)
{
	m_lpbmpinfohead = (LPBITMAPINFOHEADER )AVIStreamGetFrame(m_pgetframe,FrameNumber);  //从frame中获得bmp头信息
	ASSERT(m_lpbmpinfohead);
	pData=(BYTE *)m_lpbmpinfohead + m_lpbmpinfohead->biSize+ m_lpbmpinfohead->biClrUsed * sizeof(RGBQUAD);  //数据流也在bmp中，只是在头信息之后
	// 获取图像的像素宽、高以及每行byte数
	m_Height = m_lpbmpinfohead->biHeight;
	m_Width = m_lpbmpinfohead->biWidth;
	m_BytesPerLine=m_lpbmpinfohead->biSizeImage/m_Height;

	return (m_lpbmpinfohead)?true:false;
}

// 内联函数，不得出现判断或循环语句！
inline bool Cavi::OpenNextFrame()
{
	return (m_nCurrentFrame<m_lFrameLength-1		//若当前帧未超过上边界
		&& OpenFrame(++m_nCurrentFrame))			//尝试打开下一帧
		? true : false;								//成功则返回真，否则假
}

// 内联函数，不得出现判断或循环语句！
inline bool Cavi::OpenPreFrame()
{
	return (m_nCurrentFrame>0						//若当前帧未超过下边界
		&& OpenFrame(--m_nCurrentFrame))			//尝试打开前一帧
		? true : false;								//成功则返回真，否则假
}

// 内联函数，不得出现判断或循环语句！
inline bool Cavi::SetCurrentFrame(long newFrameIndex)
{
	return (newFrameIndex>=0						//若新值未超过下边界
		&& newFrameIndex<m_lFrameLength			//也未超过上边界
		&& (m_nCurrentFrame=newFrameIndex)>=0)	//则将新值赋给当前帧
		&& OpenFrame(m_nCurrentFrame)			//打开此帧
		? true : false;								//成功则返回真，否则假
}

// 内联函数，不得出现判断或循环语句！
inline void Cavi::ShowFrame(CDC* pDC,CRect rectShow)
{
	pDC->SetStretchBltMode(HALFTONE);
	StretchDIBits(
		pDC->GetSafeHdc(),
		rectShow.left,						//投影到窗口的坐标
		rectShow.top,
		rectShow.Width(),	
		rectShow.Height(),
		0,								//原始BMP图像范围
		0,
		m_Width,
		m_Height,
		pData,							//图像缓冲区
		(BITMAPINFO*)m_lpbmpinfohead,		//BMP图像描述信息
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

#endif 
