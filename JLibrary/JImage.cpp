// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "include/ximage.h"
#include "JImage.h"
#include "math.h"

#pragma comment(lib, "cximage.lib")

void CxImage2DC(CxImage& image, CDC& dc)
{
	CBitmap bitmap;
	bitmap.Attach(image.MakeBitmap());
	dc.SelectObject(&bitmap);
}

void DC2CxImage(CDC& dc, CxImage& image)
{
	image.CreateFromHBITMAP((HBITMAP)(*dc.GetCurrentBitmap()));
}

void CopyDC(CDC& srcDC, CDC& tarDC)
{
	CxImage image;
	DC2CxImage(srcDC, image);
	CxImage2DC(image, tarDC);
}

JImage::JImage()
: m_bOpen(false)
{
	m_memDC.CreateCompatibleDC(NULL);
}

JImage::JImage(JString fileName)
{
	m_memDC.CreateCompatibleDC(NULL);
	Load(fileName);
}

JImage::JImage(CBitmap& bitmap)
{
	m_memDC.CreateCompatibleDC(NULL);
	Load(bitmap);
}

JImage::JImage(JImage& image)
{
	m_memDC.CreateCompatibleDC(&image.m_memDC);

}

JImage::~JImage()
{
}

void JImage::operator =(JString fileName)
{
	if( m_memDC == NULL ){
		m_memDC.CreateCompatibleDC(NULL);
	}
	Load(fileName);
}

void JImage::operator =(CBitmap& bitmap)
{
	if( m_memDC == NULL ){
		m_memDC.CreateCompatibleDC(NULL);
	}
	Load(bitmap);
}

void JImage::operator =(JImage& image)
{
	Load(image);
}

void JImage::Load(JImage& image)
{
	if( m_memDC ){
		m_memDC.DeleteDC();
	}	
	m_memDC.CreateCompatibleDC(&image.m_memDC);
	m_height = image.Height;
	m_width = image.Width;
	m_bOpen = true;
}

void JImage::Load(CBitmap& bitmap)
{
	if( bitmap.GetBitmapDimension().cx != 0 && bitmap.GetBitmapDimension().cy != 0 ){
		m_memDC.SelectObject(&bitmap);
		m_height = bitmap.GetBitmapDimension().cy;
		m_width = bitmap.GetBitmapDimension().cx;
		m_bOpen = true;
	}
}

bool JImage::Load(JString fileName)
{
	if( IsFileExist(fileName) == false ){
		return false;
	}
	bool rtValue = false;
	CxImage image;
	JString type = UpperCase(Trim(ExtractFileExt(fileName), '.'));
	if( type == "BMP" ){
		rtValue = image.Load(fileName.c_str(), CXIMAGE_FORMAT_BMP);
	}else if( type == "JPG" ){
		rtValue = image.Load(fileName.c_str(), CXIMAGE_FORMAT_JPG);
	}else if( type == "PNG" ){
		rtValue = image.Load(fileName.c_str(), CXIMAGE_FORMAT_PNG);
	}
	if( rtValue ){	
		m_fileName = fileName;
		m_type = type;
		m_height = image.GetHeight();
		m_width = image.GetWidth();
		CxImage2DC(image, m_memDC);
		m_bOpen = true;
	}
	return rtValue;
}

bool JImage::Save()
{
	bool rtValue = false;
	CxImage image;
	DC2CxImage(m_memDC, image);
	if( m_type == "BMP" ){
		rtValue = image.Save(m_fileName.c_str(), CXIMAGE_FORMAT_BMP);
	}else if( m_type == "JPG" ){
		rtValue = image.Save(m_fileName.c_str(), CXIMAGE_FORMAT_JPG);
	}else if( m_type == "PNG" ){
		rtValue = image.Save(m_fileName.c_str(), CXIMAGE_FORMAT_PNG);
	}
	m_bOpen = false;
	return rtValue;
}

bool JImage::Save(JString fileName)
{
	bool rtValue = false;
	CxImage image;
	DC2CxImage(m_memDC, image);
	JString type = UpperCase(Trim(ExtractFileExt(fileName), '.'));
	if( type == "BMP" ){
		rtValue = image.Save(fileName.c_str(), CXIMAGE_FORMAT_BMP);
	}else if( type == "JPG" ){
		rtValue = image.Save(fileName.c_str(), CXIMAGE_FORMAT_JPG);
	}else if( type == "PNG" ){
		rtValue = image.Save(fileName.c_str(), CXIMAGE_FORMAT_PNG);
	}
	m_bOpen = false;
	return rtValue;
}

void JImage::Create(long width, long height)
{
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&m_memDC, width, height);
	m_memDC.SelectObject(&bitmap);
	m_memDC.FillSolidRect(0, 0, width, height, WhiteColor);
}

void JImage::GetBitmap(CBitmap& bitmap)
{
	bitmap.DeleteObject();
	bitmap.CreateCompatibleBitmap(&m_memDC, m_width, m_height);
}

void JImage::InflateRect(long l, long t, long r, long b, COLORREF color /* = WhiteColor */)
{
	if( !m_bOpen ){
		return;
	}
	long newWidth = m_width + l + r;
	long newHeight = m_height + t + b;
	CDC tmpDC;
	tmpDC.CreateCompatibleDC(NULL);
	CopyDC(m_memDC, tmpDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&tmpDC, newWidth, newHeight);
	m_memDC.SelectObject(&bitmap);
	CBrush brush;
	brush.CreateSolidBrush(color);
	m_memDC.SelectObject(&brush);
	m_memDC.FillSolidRect(0, 0, newWidth, newHeight, color);
	long x, y, x1, y1;
	if( l > 0 ){
		x = l;
		x1 = 0;
	}else{
		x = 0;
		x1 = abs(l);
	}
	if( t > 0 ){
		y = t;
		y1 = 0;
	}else{
		y = 0;
		y1 = abs(t);
	}
	m_memDC.BitBlt(x, y, newWidth, newHeight, &tmpDC, x1, y1, SRCCOPY);
	m_width = newWidth;
	m_height = newHeight;
}

void JImage::SetHeight(long height)
{
	InflateRect(0, 0, 0, height-m_height);
	m_height = height;
}

void JImage::SetWidth(long width)
{
	InflateRect(0, 0, width-m_width, 0);
	m_width = width;
}

void JImage::DrawLine(long x1, long y1, long x2, long y2, COLORREF color, long lineWidth /* = 1 */)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, lineWidth, color);
	CPen* oldPen = m_memDC.SelectObject(&pen);
	m_memDC.MoveTo(x1, y1);
	m_memDC.LineTo(x2, y2);
	m_memDC.SelectObject(oldPen);
}

void JImage::DrawCycle(long x, long y, COLORREF color, long radius, long lineWidth /* = 1 */, bool bFill /* = false */)
{
	x = Approximate<long>(x, 0, m_width);
	y = Approximate<long>(y, 0, m_height);
	CRect rect;
	rect.BottomRight() = CPoint(x+radius, y+radius);
	rect.TopLeft() = CPoint(x-radius, y-radius);
	CPen pen(PS_SOLID, lineWidth, color);
	CPen* pOldPen = m_memDC.SelectObject(&pen);
	CBrush* brush;
	if( bFill ){
		brush = new CBrush(color);
	}else{
		brush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	}
	CBrush* oldBrush = m_memDC.SelectObject(brush);
	m_memDC.SetBkMode(TRANSPARENT);
	m_memDC.Ellipse(&rect);
	m_memDC.SelectObject(pOldPen);
	m_memDC.SelectObject(oldBrush);
	if( bFill ){
		JDelete(brush);
	}
}

void JImage::DrawTriangle(long x, long y, COLORREF color, long radius, long lineWidth /* = 1 */, bool bFill /* = false */)
{
	vector<POINT> vec;
	POINT point;
	point.x = x-radius;
	point.y = y+radius;
	vec.push_back(point);
	point.x = x;
	point.y = y-radius;
	vec.push_back(point);
	point.x = x+radius;
	point.y = y+radius;
	vec.push_back(point);
	point.x = x-radius;
	point.y = y+radius;
	vec.push_back(point);
	DrawPolygon(vec, color, lineWidth, bFill);
}

void JImage::DrawPolygon(vector<POINT> vecPoints, COLORREF color, long lineWidth /* = 1 */, bool bFill /* = false */)
{
	for( vector<POINT>::iterator it = vecPoints.begin(); it != vecPoints.end(); ++it ){
		it->x = Approximate<long>(it->x, 0, m_width);
		it->y = Approximate<long>(it->y, 0, m_height);
	}
	CRgn rgn;
	rgn.CreatePolygonRgn(&vecPoints[0], vecPoints.size(), ALTERNATE);
	if( bFill ){
		CBrush brush(color);
		m_memDC.FillRgn(&rgn, &brush);
	}else{
		CPen pen;
		pen.CreatePen(PS_SOLID, lineWidth, color);
		CPen* oldPen = m_memDC.SelectObject(&pen);
		m_memDC.Polyline(&vecPoints[0], vecPoints.size());
		m_memDC.SelectObject(oldPen);
	}	
}

void JImage::DrawRect(long l, long t, long r, long b, COLORREF color, long lineWidth /* = 1 */, bool bFill /* = false */)
{
	vector<POINT> vec;
	POINT point;
	point.x = l;
	point.y = t;
	vec.push_back(point);
	point.x = r;
	point.y = t;
	vec.push_back(point);
	point.x = r;
	point.y = b;
	vec.push_back(point);
	point.x = l;
	point.y = b;
	vec.push_back(point);
	point.x = l;
	point.y = t;
	vec.push_back(point);
	DrawPolygon(vec, color, lineWidth, bFill);	
}

void JImage::DrawText(JString text, long x, long y, COLORREF color, long lHeight/* =30 */, long lWeight/* =400 */, bool bItalic/* =false */, bool bUnderline/* =false */, JString fontName /* = */ )
{
	LOGFONT font;
	ZeroMemory(&font, sizeof(LOGFONT));
	font.lfCharSet = DEFAULT_CHARSET;
	strncpy(font.lfFaceName, fontName.c_str(), LF_FACESIZE);
	font.lfHeight = lHeight;
	font.lfItalic = (BYTE)bItalic;
	font.lfUnderline = (BYTE)bUnderline;
	CFont newFont;
	newFont.CreateFontIndirect(&font);
	CFont* oldFont = m_memDC.SelectObject(&newFont);
	COLORREF oldColor = m_memDC.GetTextColor();
	m_memDC.SetTextColor(color);
	m_memDC.SetTextAlign(DT_LEFT|DT_TOP);
	m_memDC.TextOut(x, y, text.c_str());
	m_memDC.SelectObject(oldFont);
	m_memDC.SetTextColor(oldColor);
}