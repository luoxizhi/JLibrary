// LibraryTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JStringTest.h"
#include "JTimeTest.h"
#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
    JStringTest::NormalTest();
    Sleep(500);
    JTimeTest::NormalTest();
    Sleep(500);
	return 0;
}

