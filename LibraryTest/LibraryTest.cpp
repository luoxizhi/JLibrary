// LibraryTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JStringTest.h"
#include "JTimeTest.h"
#include "JFileTest.h"
#include "JUtilsTest.h"
#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
    JStringTest::NormalTest();
    Sleep(500);
    JTimeTest::NormalTest();
    Sleep(500);
    JFileTest::NormalTest();
    Sleep(500);
    JUtilsTest::NormalTest();

	return 0;
}

