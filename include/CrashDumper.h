#include <windows.h>

#ifndef _CRASH_DUMPER_H_
#define _CRASH_DUMPER_H_

class JCrashDumper
{
public:
	JCrashDumper();
	~JCrashDumper();
	static bool _PlaceHolder();

private:
	LPTOP_LEVEL_EXCEPTION_FILTER m_OriginalFilter;
	static LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);
};

namespace
{
	const bool bPlaceHolder = JCrashDumper::_PlaceHolder();
}

#endif