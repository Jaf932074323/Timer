#include "Timer/TickTimer.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace jaf
{
	CTickTimer::CTickTimer()
	{
	}

	CTickTimer::~CTickTimer() {}

	unsigned __int64 CTickTimer::GetNowTime()
	{
		return GetTickCount64();
	}
}
