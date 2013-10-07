#include "stdafx.h"
#include <windows.h>
#include <dbghelp.h>
#include "CrashDumper.h"
#include "JFile.h"

#pragma comment(lib, "dbghelp.lib")

JCrashDumper dumper;
JCrashDumper::JCrashDumper()
{
	m_OriginalFilter = SetUnhandledExceptionFilter(ExceptionFilter);
}

JCrashDumper::~JCrashDumper()
{
	SetUnhandledExceptionFilter(m_OriginalFilter);
}

LONG WINAPI JCrashDumper::ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	bool bDumpOK = false;
	DWORD dwProcess = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
	if (hProcess != INVALID_HANDLE_VALUE)
	{
		wstring strPath = JFileUtils::GetExeName()+JDateTime::Now().ToNumberWString();
        wstring strDumpFileName = strPath + L".dmp";
        HANDLE hFile = CreateFile(strDumpFileName.c_str(), FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            MINIDUMP_EXCEPTION_INFORMATION exception_information;
            exception_information.ThreadId = GetCurrentThreadId();
            exception_information.ExceptionPointers = ExceptionInfo;
            exception_information.ClientPointers = TRUE;

            if (MiniDumpWriteDump(hProcess, dwProcess, hFile, MiniDumpNormal, &exception_information, NULL, NULL))
            {
                bDumpOK = true;
            }
            CloseHandle(hFile);
        }
		CloseHandle(hProcess);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

bool JCrashDumper::_PlaceHolder() {return true;}