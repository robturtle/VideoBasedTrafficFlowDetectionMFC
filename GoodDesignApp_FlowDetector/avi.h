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
	//����
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

	//����
	bool OpenAviFile(LPCTSTR lpszPathName);		// ��AVI�ļ�
	bool CloseAvi();							// �ر�avi
	inline bool OpenFrame(int FrameNumber);		// ��ĳ֡
	inline bool OpenNextFrame();
	inline bool OpenPreFrame();
	inline void ShowFrame(CDC* pDC,CRect rectShow);	//��ʾĳ֡
	inline void SetElapse(UINT newElapse){m_nElapse=newElapse;}	//����֡��������λ��ms
	inline bool SetCurrentFrame(long newFrameIndex);				//���õ�ǰ֡
	//friend class CaviPlayer;						//�ṩavi���ŵĲ�����(δ����ʵ��)

	//����
	void FillRect(CRect rcZone, COLORREF color=RGB(0,255,0));		//����һ�������������꣺BMPͼ��Ĭ�����꣬��λ������

	//ʵ��
	Cavi();    
	virtual ~Cavi(); 
private:
	PAVIFILE	m_pavifile;					// ָ��AVI�ļ�ָ��
	AVIFILEINFO m_pavifileinfo;				// ָ��avi�ļ���Ϣ
	PAVISTREAM	m_pavistream;				// ָ��avi��
	LPBITMAPINFOHEADER m_lpbmpinfohead;	// ָ��λͼͷ��Ϣ
	PGETFRAME m_pgetframe; //
	BYTE *pData;				// �������ݵĿ�ͷָ��
	int m_Width;				// ���ؿ�
	int m_Height;				// ���ظ�
	int m_BytesPerLine;		//��Ϊÿһ��ĩβ�������������ݣ�����ÿ��byte��>=���ؿ�*
	long m_lFrameLength;		//֡����
	bool m_bOpenState;		//�򿪵�״̬
	//�������벥����صĲ���
	bool m_bHalt;
	UINT m_nElapse;			//֡��������λ����
	long m_nCurrentFrame;		//��ǰ֡��
	UINT m_nJumpStep;		//ÿ�����֡��
};

//ע�⣺�˺������ڲ�û�м����Ƶ�򿪵�״̬����ע������ڳɹ���AVI�ļ����ٵ��ô˺���
// �������������ó����жϻ�ѭ����䣡
inline bool Cavi::OpenFrame(int FrameNumber)
{
	m_lpbmpinfohead = (LPBITMAPINFOHEADER )AVIStreamGetFrame(m_pgetframe,FrameNumber);  //��frame�л��bmpͷ��Ϣ
	ASSERT(m_lpbmpinfohead);
	pData=(BYTE *)m_lpbmpinfohead + m_lpbmpinfohead->biSize+ m_lpbmpinfohead->biClrUsed * sizeof(RGBQUAD);  //������Ҳ��bmp�У�ֻ����ͷ��Ϣ֮��
	// ��ȡͼ������ؿ����Լ�ÿ��byte��
	m_Height = m_lpbmpinfohead->biHeight;
	m_Width = m_lpbmpinfohead->biWidth;
	m_BytesPerLine=m_lpbmpinfohead->biSizeImage/m_Height;

	return (m_lpbmpinfohead)?true:false;
}

// �������������ó����жϻ�ѭ����䣡
inline bool Cavi::OpenNextFrame()
{
	return (m_nCurrentFrame<m_lFrameLength-1		//����ǰ֡δ�����ϱ߽�
		&& OpenFrame(++m_nCurrentFrame))			//���Դ���һ֡
		? true : false;								//�ɹ��򷵻��棬�����
}

// �������������ó����жϻ�ѭ����䣡
inline bool Cavi::OpenPreFrame()
{
	return (m_nCurrentFrame>0						//����ǰ֡δ�����±߽�
		&& OpenFrame(--m_nCurrentFrame))			//���Դ�ǰһ֡
		? true : false;								//�ɹ��򷵻��棬�����
}

// �������������ó����жϻ�ѭ����䣡
inline bool Cavi::SetCurrentFrame(long newFrameIndex)
{
	return (newFrameIndex>=0						//����ֵδ�����±߽�
		&& newFrameIndex<m_lFrameLength			//Ҳδ�����ϱ߽�
		&& (m_nCurrentFrame=newFrameIndex)>=0)	//����ֵ������ǰ֡
		&& OpenFrame(m_nCurrentFrame)			//�򿪴�֡
		? true : false;								//�ɹ��򷵻��棬�����
}

// �������������ó����жϻ�ѭ����䣡
inline void Cavi::ShowFrame(CDC* pDC,CRect rectShow)
{
	pDC->SetStretchBltMode(HALFTONE);
	StretchDIBits(
		pDC->GetSafeHdc(),
		rectShow.left,						//ͶӰ�����ڵ�����
		rectShow.top,
		rectShow.Width(),	
		rectShow.Height(),
		0,								//ԭʼBMPͼ��Χ
		0,
		m_Width,
		m_Height,
		pData,							//ͼ�񻺳���
		(BITMAPINFO*)m_lpbmpinfohead,		//BMPͼ��������Ϣ
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

#endif 
