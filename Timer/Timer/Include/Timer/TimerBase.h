#pragma once
// 2020-12-3 姜安富
// 定时器基类
// 用一个线程去处理定时工作
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <string>
#include "TimerInterface.h"

namespace jaf
{

	const long long g_waitMaxTime = 24 * 1000 * 60 * 60; // 定时最大等待时间
	class CTimerBase :public CTimerInterface
	{
	public:
		// strName 定时器名称
		CTimerBase();
		virtual ~CTimerBase();

		// 添加定时任务
		virtual void AddTask(const STimerTask& rTask);
		// 清除所有定时任务
		virtual void Clear();

	protected:
		// 启动定时
		void Start();
		// 停止定时
		void Stop();

		// 定时器工作线程执行函数
		virtual void Work();
		// 执行定时任务
		virtual void ExecuteTask(const STimerTask& rTask);

		// 获取当前时间，不同的方式获取的时间，得到的定时精度不相同
		virtual unsigned __int64 GetNowTime() = 0;

	protected:
		// 内部使用的定时任务
		struct STimerTaskInter
		{
			STimerTask m_timerTask;
			unsigned __int64 m_nStartTime = 0; // 用于计算时间间隔的开始时间
		};

	protected:
		std::string m_strName; // 定时器名称

		std::mutex m_timerLock; // 定时任务锁
		std::mutex m_startStopWorkThreakLock; // 启停工作线程锁
		std::atomic<bool> m_bRun = false; // 工作线程运行标志
		std::thread* m_pWorkThreak = nullptr; // 工作线程
		std::condition_variable_any m_workCondition; // 定时用条件变量，用其超时特性来定时，在定时的过程中也能随时唤醒
		std::list<STimerTaskInter> m_listTimerTask; // 定时任务列表
	};
};

