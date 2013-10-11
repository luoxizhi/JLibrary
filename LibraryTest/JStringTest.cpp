// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JStringTest.h"
#include "JString.h"
#include <iostream>
using std::wcout;
using std::cout;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\JLibrary.lib")
#else
#pragma comment(lib, "..\\Release\\JLibrary.lib")
#endif

bool JStringTest::RunFunction(bool result, string strFunc)
{
    if(result == false)
    {
        cout << "Failed    " << strFunc << "\n";
    }
    else
    {
        cout << "Pass    " <<strFunc << "\n";
    }
    return result;
}

bool JStringTest::NormalTest()
{
    cout << "---------------------------------------\n";
    cout << "     File JString.h starting...\n";
    
    bool all_pass = RunFunction(TestStringCutLeft_Size(), "JStringUtils::StringCutLeft_Size") 
                 && RunFunction(TestStringCutLeft_String(), "JStringUtils::StringCutLeft_String")
                 && RunFunction(TestStringCutRight_Size(), "JStringUtils::StringCutRight_Size")
                 && RunFunction(TestStringCutRight_String(), "JStringUtils::StringCutRight_String")
                 && RunFunction(TestTrimString(), "JStringUtils::TrimString")
                 && RunFunction(TestTrimLeft(), "JStringUtils::TrimLeft")
                 && RunFunction(TestTrimRight(), "JStringUtils::TrimRight")
                 && RunFunction(TestStringSplit_Single(), "JStringUtils::StringSplit_Single")
                 && RunFunction(TestStringSplit_Multi(), "JStringUtils::StringSplit_Multi")
                 && RunFunction(TestUpperCase(), "JStringUtils::UpperCase")
                 && RunFunction(TestLowerCase(), "JStringUtils::LowerCase")
                 && RunFunction(TestStringLengthFixed(), "JStringUtils::StringLengthFixed")
                 && RunFunction(TestSubStrPairs(), "JStringUtils::SubStrPairs")
                 && RunFunction(TestStringReplaceAll_Single(), "JStringUtils::StringReplaceAll_Single")
                 && RunFunction(TestStringReplaceAll_Multi(), "JStringUtils::StringReplaceAll_Multi")
                 && RunFunction(TestStringReverse(), "JStringUtils::StringReverse")
                 && RunFunction(TestToString(), "JStringUtils::ToString")
                 && RunFunction(TestToWString(), "JStringUtils::ToWString");
    cout << "     File JString.h " << (all_pass? "PASS" : "FAILED") << "\n";
    cout << "---------------------------------------\n";
    return all_pass;
}

bool JStringTest::TestStringCutLeft_Size()
{
    wstring str1 = L"abc123";
    string str2 = "abc123";
    wstring str3 = L"你好";
    
    return JStringUtils::StringCutLeft(str1, 3) == L"123"
        && JStringUtils::StringCutLeft(str1, 0) == str1
        && JStringUtils::StringCutLeft(str1, 7) == L""
        && JStringUtils::StringCutLeft(str2, 3) == "123"
        && JStringUtils::StringCutLeft(str2, 0) == str2
        && JStringUtils::StringCutLeft(str2, 7) == ""
       /* && JStringUtils::StringCutLeft(str3, 1) == L"好"*/;
}

bool JStringTest::TestStringCutLeft_String()
{
    wstring str1 = L"abc123";
    string str2 = "abc123";

    return JStringUtils::StringCutLeft(str1, wstring(L"ab")) == L"c123"
        && JStringUtils::StringCutLeft(str1, wstring(L"")) == str1
        && JStringUtils::StringCutLeft(str1, wstring(L"abd")) == str1
        && JStringUtils::StringCutLeft(str1, wstring(L"abc123")) == L""
        && JStringUtils::StringCutLeft(str1, wstring(L"abc1234")) == str1
        && JStringUtils::StringCutLeft(str1, wstring(L"bc")) == L"123"
        && JStringUtils::StringCutLeft(str2, string("ab")) == "c123"
        && JStringUtils::StringCutLeft(str2, string("")) == str2
        && JStringUtils::StringCutLeft(str2, string("abd")) == str2
        && JStringUtils::StringCutLeft(str2, string("abc123")) == ""
        && JStringUtils::StringCutLeft(str2, string("abc1234")) == str2
        && JStringUtils::StringCutLeft(str2, string("bc")) == "123";
}

bool JStringTest::TestStringCutRight_Size()
{
    wstring str1 = L"abc123";
    string str2 = "abc123";

    return JStringUtils::StringCutRight(str1, 3) == L"abc"
        && JStringUtils::StringCutRight(str1, 0) == str1
        && JStringUtils::StringCutRight(str1, 7) == L""
        && JStringUtils::StringCutRight(str2, 3) == "abc"
        && JStringUtils::StringCutRight(str2, 0) == str2
        && JStringUtils::StringCutRight(str2, 7) == "";
}

bool JStringTest::TestStringCutRight_String()
{
    wstring str1 = L"abc123";
    string str2 = "abc123";

    return JStringUtils::StringCutRight(str1, wstring(L"23")) == L"abc1"
        && JStringUtils::StringCutRight(str1, wstring(L"")) == str1
        && JStringUtils::StringCutRight(str1, wstring(L"124")) == str1
        && JStringUtils::StringCutRight(str1, wstring(L"abc123")) == L""
        && JStringUtils::StringCutRight(str1, wstring(L"abc1234")) == str1
        && JStringUtils::StringCutRight(str1, wstring(L"12")) == L"abc"
        && JStringUtils::StringCutRight(str2, string("23")) == "abc1"
        && JStringUtils::StringCutRight(str2, string("")) == str2
        && JStringUtils::StringCutRight(str2, string("124")) == str2
        && JStringUtils::StringCutRight(str2, string("abc123")) == ""
        && JStringUtils::StringCutRight(str2, string("abc1234")) == str2
        && JStringUtils::StringCutRight(str2, string("12")) == "abc";
}

bool JStringTest::TestTrimString()
{
    wstring str1 = L" abca ";
    string str2 = " abca ";
    wstring str3 = L"回文国文回";
    string str4 = "\rabc\r\n";
    
    return JStringUtils::TrimString(wstring(str1)) == L"abca"
        && JStringUtils::TrimString(string(str2)) == "abca"
        && JStringUtils::TrimString(wstring(str1), wstring(L"a")) == L"bc"
        && JStringUtils::TrimString(string(str2), string("a")) == "bc"
        //&& JStringUtils::TrimString(wstring(str3), wstring(L"回")) == L"文国文"
        //&& JStringUtils::TrimString(wstring(str3), wstring(L"文")) == L"回文国文回"
        && JStringUtils::TrimString(string(str4)) == "abc";
}

bool JStringTest::TestTrimLeft()
{
    wstring str1 = L" abca ";
    string str2 = " abca ";
    wstring str3 = L"回文国文回";
    string str4 = "\rabc\r\n";

    return JStringUtils::LeftTrim(wstring(str1)) == L"abca "
        && JStringUtils::LeftTrim(wstring(str1), wstring(L"ab")) == L"ca "
        && JStringUtils::LeftTrim(wstring(str1), wstring(L"ca")) == L"abca "
        && JStringUtils::LeftTrim(string(str2)) == "abca "
        && JStringUtils::LeftTrim(string(str2), string("b")) == "abca "
        //&& JStringUtils::LeftTrim(wstring(str3), wstring(L"文")) == L"回文国文回"
        //&& JStringUtils::LeftTrim(wstring(str3), wstring(L"回")) == L"文国文回"
        && JStringUtils::LeftTrim(string(str4)) == "abc\r\n";
}

bool JStringTest::TestTrimRight()
{
    wstring str1 = L" abca ";
    string str2 = " abca ";
    wstring str3 = L"回文国文回";
    string str4 = "\rabc\r\n";

    return JStringUtils::RightTrim(wstring(str1)) == L" abca"
        && JStringUtils::RightTrim(wstring(str1), wstring(L"ca")) == L" ab"
        && JStringUtils::RightTrim(wstring(str1), wstring(L"ab")) == L" abca"
        && JStringUtils::RightTrim(string(str2)) == " abca"
        && JStringUtils::RightTrim(string(str2), string("a")) == " abc"
        //&& JStringUtils::RightTrim(wstring(str3), wstring(L"回")) == L"回文国文"
        //&& JStringUtils::RightTrim(wstring(str3), wstring(L"国")) == L"回文国文回"
        && JStringUtils::RightTrim(string(str4)) == "\rabc";
}

bool JStringTest::TestStringSplit_Single()
{
    bool rt = true;
    vector<wstring> buffer1;
    vector<string> buffer2;

    if(JStringUtils::StringSplit(wstring(L"ab cd ef"), wstring(L" "), buffer1) != 3){
        rt = false;
    }
    //if(JStringUtils::StringSplit(wstring(L"你好——你好——你好"), wstring(L"——"), buffer1) != 3){
    //    rt = false;
    //}
    //if(JStringUtils::StringSplit(wstring(L"你好——你好——你好——"), wstring(L"——"), buffer1) != 3){
    //    rt = false;
    //}
    //if(JStringUtils::StringSplit(wstring(L"你好——你好——你好——"), wstring(L"好"), buffer1) != 4){
    //    rt = false;
    //}
    if(JStringUtils::StringSplit(string(" ab\rcd\ref\rku"), string("\r"), buffer2) != 4){
        rt = false;
    }
    
    return rt;
}

bool JStringTest::TestStringSplit_Multi()
{
    bool rt = true;
    vector<wstring> buffer1;
    set<wstring> tags1;
    vector<string> buffer2;
    set<string> tags2;

    tags1.insert(L"1");
    tags1.insert(L"a");
    tags1.insert(L" ");
    tags1.insert(L"BBB");
    if( JStringUtils::StringSplit(wstring(L"C1BaD EBBBF"), tags1, buffer1) != 5){
        rt = false;
    }

    tags2.insert("1");
    tags2.insert("a");
    tags2.insert(" ");
    tags2.insert("BBB");
    if( JStringUtils::StringSplit(string("C1BaD EBBBF"), tags2, buffer2) != 5){
        rt = false;
    }

    //tags1.clear();
    //tags1.insert(L"你好");
    //tags1.insert(L"早");
    //if( JStringUtils::StringSplit(wstring(L"天，你好早上好啊"), tags1, buffer1) != 2){
    //    rt = false;
    //}

    return rt;
}

bool JStringTest::TestUpperCase()
{
    return JStringUtils::UpperCase(wstring(L"abc")) == L"ABC"
        && JStringUtils::UpperCase(string("abc")) == "ABC"
        /*        && JStringUtils::UpperCase(wstring(L"你ab")) == L"你AB"
        && JStringUtils::UpperCase(string("你ab")) == "你AB"*/;
}

bool JStringTest::TestLowerCase()
{
    return JStringUtils::LowerCase(wstring(L"ABC")) == L"abc"
        && JStringUtils::LowerCase(string("ABC")) == "abc"
        /*        && JStringUtils::LowerCase(wstring(L"你AB")) == L"你ab"
        && JStringUtils::LowerCase(string("你AB")) == "你ab"*/;
}

bool JStringTest::TestStringLengthFixed()
{
    return JStringUtils::StringLengthFixed(wstring(L"abc"), 5, wstring(L"0")) == L"00abc"
        //&& JStringUtils::StringLengthFixed(wstring(L"你你你"), 5, wstring(L"中国好")) == L"中国你你你"
        && JStringUtils::StringLengthFixed(wstring(L"abc"), 5, wstring(L"0"), true) == L"abc00" 
        //&& JStringUtils::StringLengthFixed(wstring(L"你你你"), 5, wstring(L"中国好"), true) == L"你你你中国"
        && JStringUtils::StringLengthFixed(string("abc"), 5, string("0")) == "00abc"
        && JStringUtils::StringLengthFixed(string("abc"), 5, string("0"), true) == "abc00";
}

bool JStringTest::TestSubStrPairs()
{
    vector<wstring> buffer1;
    vector<string> buffer2;

    return JStringUtils::StringSubPairs(wstring(L"<a><b><c>"), wstring(L"<"), wstring(L">"), buffer1) == 3
        && JStringUtils::StringSubPairs(string("<a><b><c"), string("<"), string(">"), buffer2) == 2;
}

bool JStringTest::TestStringReplaceAll_Single()
{
    return JStringUtils::StringReplaceAll(wstring(L"AAABBBCCC"), wstring(L"AAA"), wstring(L"DDD")) == L"DDDBBBCCC"
        && JStringUtils::StringReplaceAll(string("AAABBBCCC"), string("AAA"), string("DDD")) == "DDDBBBCCC";
}

bool JStringTest::TestStringReplaceAll_Multi()
{
    set<wstring> buffer1;
    buffer1.insert(L"AAA");
    buffer1.insert(L"BBB");
    set<string> buffer2;
    buffer2.insert("AAA");
    buffer2.insert("BBB");

    return JStringUtils::StringReplaceAll(wstring(L"AAABBBCCC"), buffer1, wstring(L"DDD")) == L"DDDDDDCCC"
        && JStringUtils::StringReplaceAll(string("AAABBBCCC"), buffer2, string("DDD")) == "DDDDDDCCC"
        && JStringUtils::StringReplaceAll(wstring(L"AAAABBBBCCC"), buffer1, wstring(L"DDD")) == L"DDDADDDBCCC"
        && JStringUtils::StringReplaceAll(string("AAAABBBBCCC"), buffer2, string("DDD")) == "DDDADDDBCCC";
}

bool JStringTest::TestStringReverse()
{
    return JStringUtils::StringReverse(wstring(L"aabbccdd")) == L"ddccbbaa"
        && JStringUtils::StringReverse(string("aabbccdd")) == "ddccbbaa"
        //&& JStringUtils::StringReverse(wstring(L"中国")) == L"国中"
        && JStringUtils::StringReverse(string("abcdefg"), 1, 4) == "aedcbfg";
}

bool JStringTest::TestToString()
{
    double x = 1.01;
    string str = JStringUtils::ToString(x);
    if( str != "1.01" )
        return false;

    x = -5.690;
    str = JStringUtils::ToString(x);
    if( str != "-5.69" )
        return false;

    float y = 1.01;
    str = JStringUtils::ToString(y);
    if( str != "1.01" )
        return false;

    y = 5.690;
    str = JStringUtils::ToString(y);
    if( str != "5.69" )
        return false;

    int z = 0;
    str = JStringUtils::ToString(z);
    if( str != "0" )
        return false;

    z = 65535;
    str = JStringUtils::ToString(z);
    if ( str != "65535" )
        return false;

    z = -1;
    str = JStringUtils::ToString(z);
    if( str != "-1")
        return false;
    
    long k = 109;
    str  = JStringUtils::ToString(k);
    if( str != "109" )
        return false;

    bool flag = true;
    str = JStringUtils::ToString(flag);
    if( str != "1" )
        return false;

    flag = false;
    str = JStringUtils::ToString(flag);
    if( str != "0" )
        return false;

    return true;
}

bool JStringTest::TestToWString()
{
    double x = 1.01;
    wstring str = JStringUtils::ToWString(x);
    if( str != L"1.01" )
        return false;

    x = -5.690;
    str = JStringUtils::ToWString(x);
    if( str != L"-5.69" )
        return false;

    float y = 1.01;
    str = JStringUtils::ToWString(y);
    if( str != L"1.01" )
        return false;

    y = 5.690;
    str = JStringUtils::ToWString(y);
    if( str != L"5.69" )
        return false;

    int z = 0;
    str = JStringUtils::ToWString(z);
    if( str != L"0" )
        return false;

    z = 65535;
    str = JStringUtils::ToWString(z);
    if ( str != L"65535" )
        return false;

    z = -1;
    str = JStringUtils::ToWString(z);
    if( str != L"-1")
        return false;

    long k = 109;
    str  = JStringUtils::ToWString(k);
    if( str != L"109" )
        return false;

    bool flag = true;
    str = JStringUtils::ToWString(flag);
    if( str != L"1" )
        return false;

    flag = false;
    str = JStringUtils::ToWString(flag);
    if( str != L"0" )
        return false;

    return true;
}

