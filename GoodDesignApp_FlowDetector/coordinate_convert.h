#ifndef GRAPHIC_LIUYANG_LIUYANG_H
#define GRAPHIC_LIUYANG_LIUYANG_H

#include"afxcoll.h"
#pragma once

class Ccor_convert:CObject
{
public:
	Ccor_convert(void);
	~Ccor_convert(void);
	static CPoint ptClientToBmp(CRect rcClient,CPoint ptClient/*是点相对图像显示区的坐标*/,int nBmpWidth,int nBmpHeight);
	static bool rcBmpToClient(CRect rcClient,CRect rcBmp,int nBmpWidth,int nBmpHeight,CRect& rcToStore);
	static CPoint ptBmpToClient(CRect rcClient,CPoint ptBmp,int nBmpWidth,int nBmpHeight);
	//客户区上的矩形转化成图像上的矩形
	static CRect rcClientToBmp(CRect rcZone, CRect rcClient, int nBmpWidth, int nBmpHeight);
};

#endif