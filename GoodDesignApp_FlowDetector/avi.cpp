//modified r35
//modified #3
/**********************************************************************************************
 * Last modified by Liu Yang
 * File name:		avi.cpp
 * Platform:		Microsoft MFC
 * Description:		read an avi file and display a specific frame to the given DC
 * Version:		0.1.3
 * Lisence:		GPL v2.0
 * History:
 *				2012.05.14: (v0.1.2) finished basic features;
 *				2012.08.08: (v0.1.3)
 *						rewrite function FillRect to fascilate its usage 
 *						change mord functions into inlined
 *				2012.08.13:(v0.1.3.1)
 *						bug repaired
 **********************************************************************************************/
#include "stdafx.h"
#include "avi.h"

#define DEF_ELAPSE 40		//default speed for display
#define JUMP_STEP 160

Cavi::Cavi()
:m_nCurrentFrame(0)
,m_nElapse(DEF_ELAPSE)
,m_bOpenState(false)
,m_pgetframe(NULL)
,m_lFrameLength(0)
,m_nJumpStep(JUMP_STEP)
,m_bHalt(false)
{
	AVIFileInit();          // opens AVIFile library 
}

Cavi::~Cavi()
{
	CloseAvi();
	AVIFileExit();          // releases AVIFile library

}

bool Cavi::CloseAvi()
{
	if(m_bOpenState)
	{
		AVIStreamGetFrameClose(m_pgetframe);
		AVIFileRelease(m_pavifile);  // closes the file 
	}
	return m_bOpenState=false;
}

bool Cavi::OpenAviFile(LPCTSTR lpszPathName)
{
	HRESULT hr_open,hr_info,hr_stream;   //���Ƿ��ܻ�ô���Ϣ��,һ����չ��

	hr_open = AVIFileOpen(&m_pavifile, lpszPathName, OF_SHARE_DENY_WRITE|OF_READ, 0L); 
	if (hr_open != 0)
		return m_bOpenState=false; 

	hr_info = AVIFileInfo(m_pavifile, &m_pavifileinfo, sizeof(AVIFILEINFO));
	if (hr_info != 0)
		return m_bOpenState=false; 

	hr_stream = AVIFileGetStream(m_pavifile, &m_pavistream, streamtypeVIDEO, 0);   //video������
	if (hr_stream != 0)
		return m_bOpenState=false; 

	m_pgetframe = AVIStreamGetFrameOpen(m_pavistream, NULL);   //�����л����Ϣ ����һ֡֡���
	ASSERT(m_pgetframe);
	m_lFrameLength=AVIStreamLength(m_pavistream);
	m_nCurrentFrame=0;
	OpenFrame(m_nCurrentFrame);//��ȡBMPͷ�Ա������ؿ�ߡ�ÿ�б���������Ϣ
	return m_bOpenState=true;
}


//�����ú���������ȷ�����궨λ�Ƿ���ȷ
//�޸ģ����������Ӧ��С˳��ͬ��������������Ǿ���
#ifdef _DEBUG
void Cavi::FillRect(CRect rcZone,COLORREF color)
{
	CPoint ptTopLeft=rcZone.TopLeft();
	CPoint ptBottomRight=rcZone.BottomRight();
	//��֤TopLeft�����½Ƕ�BottomRight�����Ͻ�
	if(ptTopLeft.x > ptBottomRight.x){ int tmp=ptTopLeft.x; ptTopLeft.x=ptBottomRight.x; ptBottomRight.x=tmp;}
	if(ptTopLeft.y > ptBottomRight.y){ int tmp=ptTopLeft.y; ptTopLeft.y=ptBottomRight.y; ptBottomRight.y=tmp;}
	int nRectWidth=ptBottomRight.x-ptTopLeft.x,nRectHeight=ptBottomRight.y-ptTopLeft.y;
	for(int i=0;i<nRectHeight;i++)
	{
		BYTE *pTemp=pData+ptTopLeft.y*m_BytesPerLine+ptTopLeft.x*3;
		for(int j=0; j<nRectWidth; j++)
		{
			//Ϊ�����ص���ɫ
			*(pTemp++)=(BYTE)(color>>16);
			*(pTemp++)=(BYTE)(color>>8);
			*(pTemp++)=(BYTE)(color);
		}
		ptTopLeft.y++;
	}
}
#endif
