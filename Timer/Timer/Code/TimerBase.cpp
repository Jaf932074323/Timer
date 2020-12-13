#include "Timer/TimerBase.h"
#include <assert.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace jaf
{
	CTimerBase::CTimerBase()
	{
	}

	CTimerBase::~CTimerBase()
	{
		Stop();
	}

	void CTimerBase::AddTask(const STimerTask& rTask)
	{
		STimerTaskInter task;
		task.m_timerTask = rTask;
		task.m_nStartTime = GetNowTime(); // 记录最开始的时间

		{
			std::unique_lock<std::mutex> lks(m_timerLock);
			m_listTimerTask.push_back(task); // 在列表中加入定时任务
		}

		Start();

		m_workCondition.notify_all();
	}

	void CTimerBase::Clear()
	{
		Stop();
		std::unique_lock<std::mutex> lks(m_timerLock);
		m_listTimerTask.clear();
	}

	void CTimerBase::Start()
	{
		std::unique_lock<std::mutex> lks(m_startStopWorkThreakLock);
		if (m_pWorkThreak != nullptr)
		{
			return;
		}
		m_bRun = true;

		m_pWorkThreak = new std::thread(&CTimerBase::Work, this);
	}

	void CTimerBase::Stop()
	{
		std::unique_lock<std::mutex> lks(m_startStopWorkThreakLock);
		if (m_pWorkThreak == nullptr)
		{
			return;
		}
		m_bRun = false;

		m_workCondition.notify_all(); // 激活工作线程，避免工作线程阻塞
		m_pWorkThreak->join();
		delete m_pWorkThreak;
		m_pWorkThreak = nullptr;
	}

	void CTimerBase::Work()
	{
		long long nMinWaitTime = 0; // 所有任务中最小的等待时间，用于计算需要睡眠多久
		long long hadWaitTime = 0; // 一个任务已经等待了的时间
		long long nNeedWaitTime = 0; // 一个任务还需要等待的时间
		std::unique_lock<std::mutex> lks(m_timerLock);
		while (m_bRun)
		{
			m_workCondition.wait_for(m_timerLock, std::chrono::milliseconds(nMinWaitTime));
			if (!m_bRun)
			{
				return;
			}

			nMinWaitTime = g_waitMaxTime;
			unsigned __int64 nNowTime = GetNowTime();
			for (std::list<STimerTaskInter>::iterator it = m_listTimerTask.begin(); it != m_listTimerTask.end(); ++it)
			{
				hadWaitTime = nNowTime - it->m_nStartTime; // 已经等待了的时间
				if (hadWaitTime < 0) // 不能小于0
				{
					hadWaitTime = 0;
				}

				if ((hadWaitTime + m_nLeadTime) >= it->m_timerTask.m_nInterval) // 时间间隔大于指定间隔时执行 可以提前5ms执行
				{
					it->m_nStartTime = nNowTime;
					ExecuteTask(it->m_timerTask);
					nNeedWaitTime = it->m_timerTask.m_nInterval;
				}
				else
				{
					nNeedWaitTime = it->m_timerTask.m_nInterval - hadWaitTime; // 一个任务还需要等待的时间
				}
				assert(nNeedWaitTime >= 0); // 按照逻辑，nNeedWaitTime必然大于0

				// 找到最小的需要等待的时间
				if (nNeedWaitTime < nMinWaitTime)
				{
					nMinWaitTime = nNeedWaitTime;
				}
			}

		}
	}

	void CTimerBase::ExecuteTask(const STimerTask& rTask)
	{
		if (!rTask.m_fun)
		{
			return;
		}
		rTask.m_fun();
	}

};
