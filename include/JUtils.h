// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JUtils_H
#define JUtils_H

#include "JTime.h"
#include "JString.h"

#define JDelete(x) if( x ){ delete x; x = NULL; }

class JUtils
{
public:
    static void		WindowsStartUp();
    
    static __int64  ConvertTime(string time);
    static __int64  ConvertTime(wstring time);
    
    static __int64  ConvertSize(string size);
    static __int64  ConvertSize(wstring size);

    static wstring	AsciiToUnicodeString(string str);
    static string	UnicodeToAsciiString(wstring str);
};

typedef struct _LogData{
    JDateTime	dt;
    wstring		text;
}LogData;

class JLogUtils
{
public:
    static wstring	LogData2String(LogData data);
    static bool	    String2LogData(wstring strLine, LogData& data);
};

class JNumberUtils
{
public:
    template <class T> static T Round(T t, T minValue, T maxValue){
        if( t < minValue ){
            t = minValue;
        }
        if( t > maxValue ){
            t = maxValue;
        }
        return t;
    }
};


#define BlackColor		RGB(0, 0, 0)
#define PinkColor		RGB(255,192,203)
#define PurpleColor		RGB(128,0,128)
#define BlueColor		RGB(0,0,255)
#define CyanColor		RGB(0,255,255)
#define GreenColor		RGB(0,255,0)
#define YellowColor		RGB(255,255,0)
#define OrangeColor		RGB(255,165,0)
#define RedColor		RGB(255,0,0)
#define WhiteColor		RGB(255,255,255)
#define GreyColor		RGB(169,169,169)
#define LightGreyColor	RGB(211,211,211)
#define DefaultColor	(-1)

// format convert function
RGBQUAD COLORREF2RGBQUAD(COLORREF color);
COLORREF ConverseRGB(COLORREF color);

#endif // JUtils_H