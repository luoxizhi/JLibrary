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
#include <locale>

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
        return str.size() > len? str.substr(len) : T();
    }

    template<class T> static T StringCutLeft(const T str, T pattern)
    {
        if( str.size() >= pattern.size()){
            size_t pos = str.find(pattern);
            if( pos != T::npos ){
                return str.substr(pos+pattern.size());
            }
        }
        return str;
    }

    template<class T> static T StringCutRight(const T str, size_t len)
    {
        return str.size() > len? str.substr(0, str.size()-len) : T();
    }

    template<class T> static T StringCutRight(const T str, T pattern)
    {
        if( str.size() >= pattern.size() ){
            size_t pos = str.rfind(pattern);
            if( pos != T::npos ){
                return str.substr(0, pos);
            }
        }
        return str;
    }

    template<class T> static T TrimString(T& str, T pattern = T())
    {
        return LeftTrim(RightTrim(str, pattern), pattern);
    }

    template<class T> static T LeftTrim(T& str, T pattern = T())
    {
        if( !str.empty() ){
            T::iterator it;
            for( it = str.begin(); it != str.end(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t')) ){
                    break;
                }
            }
            str.erase(str.begin(), it);
        }

        size_t len = pattern.size();
        while(len && str.size() >= len){
            if(str.substr(0, len) == pattern)
                str = str.substr(len);
            else
                break;
        }
        return str;
    }

    template<class T> static T RightTrim(T& str, T pattern = T())
    {
        if( !str.empty() ){
            T::reverse_iterator it;
            for( it = str.rbegin(); it != str.rend(); ++it ){
                if( !(*it == _T(' ') || *it == _T('\r') || *it == _T('\n') || *it == _T('\t')) ){
                    break;
                }
            }
            T::difference_type dt = str.rend() - it;
            str.erase( str.begin() + dt, str.end() );
        }

        size_t len = pattern.size();
        while(len && str.size() >= len){
            if(str.substr(str.size()-len) == pattern)
                str = str.substr(0, str.size()-len);
            else
                break;
        }
        return str;
    }

    template<class T> static unsigned int StringSplit(const T str, const T tag, vector<T>& vecStr, bool bTrim = true)
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
            if( !sub_str.empty() ){
                vecStr.push_back(sub_str);
            }			
            search_index = index + tag.size();
        }

        sub_str = str.substr(search_index);
        if( bTrim ){
            sub_str = TrimString(sub_str);
        }

        if( !sub_str.empty() ){
            vecStr.push_back(sub_str);
        }

        return vecStr.size();
    }

    template<class T> static size_t	StringFind(const T str, size_t index, const set<T>& tags, T& tag)
    {
        size_t pos_front = T::npos;
        for (set<T>::const_iterator it = tags.begin(); it != tags.end(); ++it){
            size_t pos = str.find(*it, index);
            if( pos != T::npos && (pos_front == T::npos || pos < pos_front)){
                pos_front = pos;
                tag = *it;
            }
        }
        return pos_front;
    }

    template<class T> static unsigned int StringSplit(const T str, const set<T>& tags, vector<T>& vecStr, bool bTrim = true)
    {
        vecStr.clear();
        size_t index, search_index = 0;
        T sub_str;
        T tag;

        while( (index = StringFind(str, search_index, tags, tag)) != T::npos )
        {
            sub_str = str.substr(search_index, index-search_index);
            if( bTrim ){
                sub_str = TrimString(sub_str);
            }
            if( !sub_str.empty() ){
                vecStr.push_back(sub_str);
            }			
            search_index = index + tag.size();
        }

        sub_str = str.substr(search_index);
        if( bTrim ){
            sub_str = TrimString(sub_str);
        }

        if( !sub_str.empty() ){
            vecStr.push_back(sub_str);
        }

        return vecStr.size();
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

    template<class T> static T StringLengthFixed(T str, size_t fixedLength, T strFill, bool bAlignRight = false)
    {
        if( str.size() == fixedLength ){
            return str;
        }

        if( str.size() > fixedLength ){
            bAlignRight? str = str.substr(str.size()-fixedLength) :  str = str.substr(0, fixedLength);
        }else{
            size_t len_left = fixedLength-str.size();
            while(len_left >= strFill.size()){
                bAlignRight? str += strFill : str = strFill + str;
                len_left -= strFill.size();
            }
            if(len_left){
                bAlignRight? str += strFill.substr(0, len_left) : str = strFill.substr(0, len_left) + str;
            }
        }

        return str;
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

    template<class T> static T StringReverse(T& str, size_t start_pos = 0, size_t end_pos = T::npos)
    {
        if(str.length() != 0){
            if( end_pos == T::npos || end_pos >= str.length() )
                end_pos = str.length()-1;

            while( end_pos > start_pos ){
                std::swap(str[start_pos], str[end_pos]);
                start_pos++;
                end_pos--;
            }
        }

        return str;
    }

    template<class T> static unsigned int StringSubPairs(T strIn, const T first, const T second, vector<T>& out, bool self = false)
    {
        T tmp;
        size_t offset = 0;
        while(true){
            offset = SubStrPair(strIn, offset, first, second, tmp, self);
            if(offset == T::npos)
                break;
            out.push_back(tmp);
        }

        return out.size();
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

private:
        template<class T> static size_t	SubStrPair(T srcIn, size_t offset, const T first, const T second, T& out, bool self = false)
        {
            size_t index1 = srcIn.find(first, offset);
            size_t index2 = srcIn.find(second, offset);
            size_t index1_len = first.size();
            size_t index2_len = second.size();

            if( index1 != T::npos && index2 != T::npos ){
                if( index1 > index2 ){
                    std::swap(index1, index2);
                    std::swap(index1_len, index2_len);
                }
                if( self ){
                    out = srcIn.substr(index1, index2+index2_len-index1);
                }else{
                    out = srcIn.substr(index1+index1_len, index2-index1-index1_len);
                }			
                return index2+index2_len;
            }

            return T::npos;
        }
};

#endif
