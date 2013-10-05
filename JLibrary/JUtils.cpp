// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JUtils.h"

#include "JFile.h"
#include "ctype.h"
#include "windows.h"

wstring	JUtils::AsciiToUnicodeString(string str)
{
	long len = str.length();
	long unicode_len = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wstring rt;
	WCHAR* pUnicode = new WCHAR[unicode_len+1];

	if( pUnicode ){
		ZeroMemory(pUnicode, (unicode_len+1)*sizeof(WCHAR));
		::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicode_len);
		rt = pUnicode;
		delete pUnicode;
	}	
	return rt;
}

string JUtils::UnicodeToAsciiString(wstring str)
{
	long utf_len = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);	
	string rt;
    char* pData = new char[utf_len+1];

	if( pData ){
		ZeroMemory(pData, utf_len+1);
		::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, pData, utf_len, NULL, 0);
		rt = pData;
		delete pData;
	}	
	return rt;
}

void JUtils::WindowsStartUp()
{
	HKEY hKey; 
	LPCWSTR lpRun = L"Software//Microsoft//Windows//CurrentVersion//Run"; 
	long lRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey); 
	if(lRet == ERROR_SUCCESS) 
	{ 
		WCHAR pFileName[MAX_PATH] = {0}; 
		wstring fileName = JFileUtils::GetExeName();
		lRet = RegSetValueExW(hKey, JFileUtils::ExtractFileNameNoExt(fileName).c_str(), 0, REG_SZ, (BYTE*)fileName.c_str(), (DWORD)fileName.length());
		RegCloseKey(hKey); 
	}
}

__int64 JUtils::ConvertTime(string time)
{
    time = JStringUtils::UpperCase(time);
    size_t pos = -1;
    __int64 val = 0;
    for(size_t i = 0; i < time.size(); i++){
        if( time[i] == 'D' || time[i] == 'H' || time[i] == 'M' || time[i] == 'S'){
            long tmp = atol(time.substr(pos+1, i-pos-1).c_str());
            if( time[i] == 'D' ){
                val += tmp * JDateTime_Day_Sec;
            }else if( time[i] == 'H' ){
                val += tmp * JDateTime_Hour_Sec;
            }else if( time[i] == 'M' ){
                val += tmp * JDateTime_Minute_Sec;
            }else if( time[i] == 'S' ){
                val += tmp * JDateTime_Sec;
            }
        }	
    }
    return val;
}

__int64 JUtils::ConvertTime(wstring time)
{
    return ConvertTime(UnicodeToAsciiString(time));
}

__int64 JUtils::ConvertSize(string size)
{
    size = JStringUtils::UpperCase(size);
    __int64 val = -1;
    for(size_t i = 0; i < size.size(); i++){
        if( !isdigit(size[i]) ){
            val = atol(size.substr(0, i).c_str());
            switch(size[i]){
            case 'G':	val *= 1024*1024*1024; break;
            case 'M':	val *= 1024*1024; break;
            case 'K':	val *= 1024; break;
            default:	break;
            }
            break;
        }
        if( i == size.size()-1 ){
            val = atol(size.c_str());
        }
    }
    return val;
}

__int64 JUtils::ConvertSize(wstring size)
{
    return ConvertSize(UnicodeToAsciiString(size));
}

bool JLogUtils::String2LogData(wstring strLine, LogData& data)
{
	if( strLine.length() >= JDateTime::DataTimeStringLength ){
		data.dt = JDateTime(strLine.substr(0, JDateTime::DataTimeStringLength));
		data.text = strLine.substr(JDateTime::DataTimeStringLength);
		return true;
	}
	return false;	
}

wstring JLogUtils::LogData2String(LogData data)
{
	return data.dt.ToWString() + _T("  ") + data.text;
}

// format convert function
RGBQUAD COLORREF2RGBQUAD(COLORREF color)
{
	RGBQUAD rgbQuad;
	rgbQuad.rgbBlue = GetBValue(color);
	rgbQuad.rgbGreen = GetGValue(color);
	rgbQuad.rgbRed = GetRValue(color);
	return rgbQuad;
}

COLORREF ConverseRGB(COLORREF color)
{
	return RGB(255-GetRValue(color), 255-GetGValue(color), 255-GetBValue(color));
}
