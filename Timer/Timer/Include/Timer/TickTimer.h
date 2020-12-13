#pragma once
// 2020-12-3 姜安富
// Tick定时器
// 使用GetTickCount64()的方式去获取系统时间，
#include "TimerBase.h"

namespace jaf
{
	class CTickTimer :public CTimerBase
	{
	public:
		CTickTimer();
		virtual ~CTickTimer();

	protected:
		// 获取当前时间，不同的方式获取的时间，得到的定时精度不相同
		virtual unsigned __int64 GetNowTime();
	};
}

