// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JStringUtils_H
#define JStringUtils_H

#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

using std::wostringstream;
using std::ostringstream;
using std::vector;
using std::set;
using std::string;
using std::wstring;

class JStringUtils
{
public:
    template<class T> static T StringCutLeft(const T str, size_t len)
    {
        return str.size() > len? str.substr(len) : str;
    }

    template<class T> static T StringCutLeft(const T str, T pattern)
    {
        size_t pos = str.find(pattern);
        return pos != T::npos? str.substr(0, pos) : str;
    }

    template<class T> static T StringCutRight(const T str, size_t len)
    {
        return str.size() > len? str.substr(0, str.size()-len) : str;
    }

    template<class T> static T StringCutRight(const T str, T pattern)
    {
        size_t pos = str.rfind(pattern);
        return pos != T::npos? str.substr(pos+1) : str;
    }

    template<class T> static void StringSplit(const T str, const T tag, vector<T>& vecStr, bool bTrim = true)
    {
        vecStr.clear();
        size_t index, search_index = 0;
        T sub_str;

        while( (index = str.find(tag, search_index)) != T::npos )
        {
            sub_str = str.substr(search_index, index-search_index);
            if( bTrim ){
                sub_str = TrimString(sub_str);
            }
            if( sub_str != _T("") ){
                vecStr.push_back(sub_str);
            }			
            search_index = index + 1;
        }

        sub_str = str.substr(search_index);
        if( bTrim ){
            sub_str = TrimString(sub_str);
        }

        if( sub_str != _T("") ){
            vecStr.push_back(sub_str);
        }		
    }

    template<class T> static void StringSplit(const T str, const set<T>& tags, vector<T>& vecStr, bool bTrim = true)
    {
        vecStr.clear();
        size_t index, search_index = 0;
        T sub_str;

        while( (index = StringFind(str, search_index, tags)) != T::npos )
        {
            sub_str = str.substr(search_index, index-search_index);
            if( bTrim ){
                sub_str = TrimString(sub_str);
            }
            if( !sub_str.empty() ){
                vecStr.push_back(sub_str);
            }			
            search_index = index + 1;
        }

        sub_str = str.substr(search_index);
        if( bTrim ){
            sub_str = TrimString(sub_str);
        }

        if( !sub_str.empty() ){
            vecStr.push_back(sub_str);
        }
    }

    template<class T> static size_t	StringFind(const T str, size_t index, const set<T>& tags)
    {
        for (set<T>::const_iterator it = tags.begin(); it != tags.end(); ++it){
            size_t pos = str.find(*it, index);
            if( pos != T::npos ){
                return pos;
            }
        }
        return T::npos;
    }

    template<class T> static T StringLengthFixed(T str, size_t fixedLength, TCHAR strFill =_T(' '), bool bAlignRight = false)
    {
        if( str.size() == fixedLength ){
            return str;
        }
        if( str.size() > fixedLength ){
            if( bAlignRight ){
                str = str.substr(str.size()-fixedLength);
            }else{
                str = str.substr(0, fixedLength);
            }
        }else{
            if( bAlignRight ){
                str.insert(0, str.size()-fixedLength, strFill);
            }else{
                str += T(str.size()-fixedLength, strFill);
            }
        }
        return str;
    }

    template<class T> static T TrimString(T str, TCHAR pattern = _T(' '))
    {
        if( !str.empty() ){
            T::iterator it;
            for( it = str.begin(); it != str.end(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t') || *it == pattern) ){
                    break;
                }
            }
            str.erase(str.begin(), it);
        }
        if( !str.empty() ){
            T::reverse_iterator it;
            for( it = str.rbegin(); it != str.rend(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t') || *it == pattern) ){
                    break;
                }
            }
            T::difference_type dt = str.rend() - it;
            str.erase( str.begin() + dt, str.end() );
        }
        return str;
    }

    template<class T> static T LeftTrim(T str, TCHAR pattern = _T(' '))
    {
        if( !str.empty() ){
            T::iterator it;
            for( it = str.begin(); it != str.end(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t') || *it == pattern) ){
                    break;
                }
            }
            str.erase(str.begin(), it);
        }
        return str;
    }

    template<class T> static T RightTrim(T str, TCHAR pattern = _T(' '))
    {
        if( !str.empty() ){
            T::reverse_iterator it;
            for( it = str.rbegin(); it != str.rend(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t') || *it == pattern) ){
                    break;
                }
            }
            T::difference_type dt = str.rend() - it;
            str.erase( str.begin() + dt, str.end() );
        }
        return str;
    }

    template<class T> static T UpperCase(T str)
    {
        for( size_t i = 0; i < str.size(); ++i ){
            str[i] = toupper(str[i]);
        }
        return str;
    }

    template<class T> static T LowerCase(T str)
    {
        for( size_t i = 0; i < str.size(); ++i ){
            str[i] = tolower(str[i]);
        }
        return str;
    }

    template<class T> static size_t	SubStrPair(T srcIn, size_t offset, const T first, const T second, T& out, bool self = false)
    {
        size_t index1 = srcIn.find(first, offset);
        size_t index2 = srcIn.find(second, offset);
        size_t index1_len = first.size();
        size_t index2_len = second.size();
        if( index1 != T::npos && index2 != T::npos ){
            if( index1 > index2 ){
                swap(index1, index2);
                swap(index1_len, index2_len);
            }
            if( self ){
                out = srcIn.substr(index1, index2+index2_len-index1);
            }else{
                out = srcIn.substr(index1+index1_len, index2-index1-index1_len);
            }			
            return index2+index2_len+1;
        }
        return T::npos;
    }

    template<class T> static T StringReplaceAll(T strOriginal, T strOldPattern, T strNewPattern)
    {
        if( strOldPattern == strNewPattern )
            return strOriginal;

        size_t index = 0;
        T newString = strOriginal;

        size_t pos = UpperCase(newString).find(UpperCase(strOldPattern));
        while( pos >= index && pos != T::npos )
        {
            newString.replace(pos, strOldPattern.length(), strNewPattern);
            index = pos + strNewPattern.length();
            pos = UpperCase(newString).find(UpperCase(strOldPattern), index);
        }

        return newString;
    }

    template<class T> static T StringReplaceAll(T strOriginal, set<T>& setOldPattern, T strNewPattern)
    {
        if( setOldPattern.empty() )
            return strOriginal;

        for( set<T>::iterator it = setOldPattern.begin(); it != setOldPattern.end(); ++it ){
            strOriginal = StringReplaceAll(strOriginal, *it, strNewPattern);
        }

        return strOriginal;
    }

    template<class T> static void StringReverse(T& str, size_t start_pos = 0, size_t end_pos = T::npos)
    {
        if(str.length() == 0)
            return;

        if( end_pos == T::npos || end_pos >= str.length() )
            end_pos = str.length()-1;

        while( end_pos > start_pos ){
            swap(str[start_pos], str[end_pos]);
            start_pos++;
            end_pos--;
        }
    }

    template <typename T> static wstring ToWString(T t){
        wostringstream oss(wostringstream::out);
        oss << t;
        return oss.str();
    }

    template <typename T> static string ToString(T t){
        ostringstream oss(ostringstream::out);
        oss << t;
        return oss.str();
    }

    template <typename T> wstring ToWStringFixedLength(T t, long len, WCHAR filled_char, bool align_right = false){
        wstring str = ToWString<T>(t);
        return StringLengthFixed<wstring>(str, len, filled_char, align_right);
    }

    template <class T> wstring ToWStringHex(T t){
        wostringstream oss(wostringstream::out);
        oss.setf ( std::ios::hex, std::ios::basefield );
        oss.setf ( std::ios::showbase );
        oss << t;
        return oss.str();
    }

    template <class T> string ToStringHex(T t){
        ostringstream oss(ostringstream::out);
        oss.setf ( std::ios::hex, std::ios::basefield );
        oss.setf ( std::ios::showbase );
        oss << t;
        return oss.str();
    }
};

#endif
