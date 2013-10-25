#include "stdafx.h"
#include "coordinate_convert.h"

Ccor_convert::Ccor_convert(void)
{
}

Ccor_convert::~Ccor_convert(void)
{
}

CPoint Ccor_convert::ptClientToBmp(CRect rcClient,CPoint ptClient,int nBmpWidth,int nBmpHeight)
{
	CPoint ptBmp;
	ptBmp.x=ptClient.x*nBmpWidth/rcClient.Width();
	ptBmp.y=nBmpHeight-ptClient.y*nBmpHeight/rcClient.Height();
	return ptBmp;
}

CPoint Ccor_convert::ptBmpToClient(CRect rcClient,CPoint ptBmp,int nBmpWidth,int nBmpHeight)
{
	CPoint ptClient;
	ptClient.x=ptBmp.x*rcClient.Width()/nBmpWidth;
	ptClient.y=(nBmpHeight-ptBmp.y)*rcClient.Height()/nBmpHeight;
	return ptClient;
}

#define swap(a,b,T)	{T tmp=a; a=b; b=tmp;}
CRect Ccor_convert::rcClientToBmp(CRect rcZone, CRect rcClient, int nBmpWidth, int nBmpHeight)
{
	CRect rcBmp;
	rcBmp.left = rcZone.left * nBmpWidth / rcClient.Width();
	rcBmp.right = rcZone.right * nBmpWidth / rcClient.Width();
	rcBmp.top = nBmpHeight - rcZone.bottom * nBmpHeight / rcClient.Height();
	rcBmp.bottom = nBmpHeight - rcZone.top * nBmpHeight / rcClient.Height();
	//确保矩形坐标为正
	if(rcBmp.left > rcBmp.right) swap(rcBmp.left, rcBmp.right, int);
	if(rcBmp.top > rcBmp.bottom) swap(rcBmp.top, rcBmp.bottom, int);
	return rcBmp;
}

bool Ccor_convert::rcBmpToClient(CRect rcClient,CRect rcBmp,int nBmpWidth,int nBmpHeight,CRect& rcToStore)
{
	CPoint pt1=rcBmp.TopLeft(),pt2=rcBmp.BottomRight();
	pt1=ptBmpToClient(rcClient,rcBmp.TopLeft(),nBmpWidth,nBmpHeight);
	pt2=ptBmpToClient(rcClient,rcBmp.BottomRight(),nBmpWidth,nBmpHeight);
	rcToStore.left=pt1.x;
	rcToStore.right=pt2.x;
	rcToStore.top=pt2.y;
	rcToStore.bottom=pt1.y;
	//返回值指示坐标系是否为正
	if(pt1.x<pt2.x && pt2.y<pt1.y)
		return true;
	else
		return false;
}
