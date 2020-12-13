#include "Timer/HighPrecisionTimer.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace jaf
{
	CHighPrecisionTimer::CHighPrecisionTimer()
	{
		QueryPerformanceFrequency(&m_cpuFreq);
	}

	CHighPrecisionTimer::~CHighPrecisionTimer() {}

	unsigned __int64 CHighPrecisionTimer::GetNowTime()
	{
        LARGE_INTEGER time;
        double rumTime = 0.0;
		QueryPerformanceCounter(&time);
		rumTime = (time.QuadPart * 1000.0f) / m_cpuFreq.QuadPart;

		return (unsigned __int64)rumTime;
	}

}
