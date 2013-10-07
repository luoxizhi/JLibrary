// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "JUtils.h"

#ifndef JImage_H
#define JImage_H

class JImage
{
public:
	__declspec(property(get=GetHeight,	put=SetHeight))			long	Height;
	__declspec(property(get=GetWidth,	put=SetWidth))			long	Width;
	long	GetHeight(){ return m_height; }
	long	GetWidth(){ return m_width; }
	void	SetHeight(long height);
	void	SetWidth(long width);

public:
	JImage();
	JImage(JString fileName);
	JImage(CBitmap& bitmap);
	JImage(JImage& image);
	~JImage();

	bool	Load(JString fileName);
	void	Load(CBitmap& bitmap);
	void	Load(JImage& image);
	bool	Save(JString fileName);
	bool	Save();
	void	Create(long width, long height);
	void	GetBitmap(CBitmap& bitmap);
	CDC*	GetDC(){ return &m_memDC; }
	// origin of coordinates is LeftTopf
	void	InflateRect(long l, long t, long r, long b, COLORREF color = WhiteColor);
	void	DrawLine(long x1, long y1, long x2, long y2, COLORREF color, long lineWidth = 1);
	void	DrawCycle(long x, long y, COLORREF color, long radius, long lineWidth = 1, bool bFill = false);
	void	DrawTriangle(long x, long y, COLORREF color, long radius, long lineWidth = 1, bool bFill = false);
	void	DrawRect(long l, long t, long r, long b, COLORREF color, long lineWidth = 1, bool bFill = false);
	void	DrawPolygon(vector<POINT> vecPoints, COLORREF color, long lineWidth = 1, bool bFill = false);
	void	DrawText(JString text, long x, long y, COLORREF color, long lHeight=30, long lWeight=400, bool bItalic=false, bool bUnderline=false, JString fontName = "Arial");

	void	operator = (JString fileName);
	void	operator = (CBitmap& bitmap);
	void	operator = (JImage& image);

private:
	CDC		m_memDC;
	JString  m_type;
	JString  m_fileName;
	bool	m_bOpen;
	long	m_width;
	long	m_height;
};

#endif
