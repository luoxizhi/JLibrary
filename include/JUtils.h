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
#include <string>
#include <ios>
using std::wstring;
using std::string;

#define JDelete(x) if( x ){ delete x; x = NULL; }

class JUtils
{
public:
    static void			WindowsStartUp();
    
    template<class T> static __int64 ConvertTime(T time)
    {
        time = JStringUtils::UpperCase<T>(time);
        size_t pos = -1;
        __int64 val = 0;
        for(size_t i = 0; i < time.size(); i++){
            if( time[i] == _T('D') || time[i] == _T('H') || time[i] == _T('M') || time[i] == _T('S')){
                long tmp = _tstoi(time.substr(pos+1, i-pos-1).c_str());
                if( time[i] == _T('D') ){
                    val += tmp * JDateTime_Day_Sec;
                }else if( time[i] == _T('H') ){
                    val += tmp * JDateTime_Hour_Sec;
                }else if( time[i] == _T('M') ){
                    val += tmp * JDateTime_Minute_Sec;
                }else if( time[i] == _T('S') ){
                    val += tmp * JDateTime_Sec;
                }
            }	
        }
        return val;
    }
    template<class T> static __int64 ConvertSize(T size)
    {
        size = JStringUtils::UpperCase<T>(size);
        __int64 val = -1;
        for(size_t i = 0; i < size.size(); i++){
            if( !isdigit(size[i]) ){
                val = _tstoi(size.substr(0, i).c_str());
                switch(size[i]){
                case _T('G'):	val *= 1024*1024*1024; break;
                case _T('M'):	val *= 1024*1024; break;
                case _T('K'):	val *= 1024; break;
                default:	break;
                }
                break;
            }
            if( i == size.size()-1 ){
                val = _tstoi(size.c_str());
            }
        }
        return val;
    }

    static wstring		AsciiToUnicodeString(string str);
    static string		UnicodeToAsciiString(wstring str);

    template <class T> static void swap(T& t1, T& t2){
        T tmp = t1;
        t1 = t2;
        t2 = tmp;
    }
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
    template <class T> static T Approximate(T t, T minValue, T maxValue){
        if( t < minValue ){
            t = minValue;
        }
        if( t > maxValue ){
            t = maxValue;
        }
        return t;
    }

    template <class T> T FromWString(wstring str){
        T t;
        wistringstream iss(str, wistringstream::in);
        iss >> t;
        return t;
    }

    template <class T> T FromString(string str){
        T t;
        istringstream iss(str, istringstream::in);
        iss >> t;
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