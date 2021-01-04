// MIT License
//
// Copyright(c) 2021 Jaf932074323
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

	unsigned int CTimerBase::AddTask(unsigned int nTimeId, const STimerTask& rTask)
	{
		assert(rTask.m_nInterval != 0); // 定时间隔不能为0
		if (nTimeId == 0) // 定时Id不能为0
		{
			return 0;
		}

		STimerTaskInter task;
		task.m_timerTask = rTask;
		task.m_nStartTime = GetNowTime(); // 记录最开始的时间

		{
			std::unique_lock<std::mutex> lks(m_timerLock);
			if (m_mapTimerTask.find(nTimeId) != m_mapTimerTask.end())
			{
				return 0;
			}
			m_mapTimerTask.insert(std::make_pair(nTimeId, task));
		}

		Start();
		m_workCondition.notify_all();

		return nTimeId;
	}

	void CTimerBase::Clear()
	{
		Stop();
		std::unique_lock<std::mutex> lks(m_timerLock);
		m_mapTimerTask.clear();
	}

	void CTimerBase::Remove(unsigned int nTimeId)
	{
		std::unique_lock<std::mutex> lks(m_timerLock);
		m_mapTimerTask.erase(nTimeId);

		m_workCondition.notify_all(); // 激活工作线程，避免工作线程阻塞
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
		unsigned __int64 nMinWaitTime = 0; // 所有任务中最小的等待时间，用于计算下一次执行需要多久
		std::unique_lock<std::mutex> lks(m_timerLock);
		while (m_bRun)
		{
			m_workCondition.wait_for(m_timerLock, std::chrono::milliseconds(nMinWaitTime));
			if (!m_bRun)
			{
				return;
			}

			nMinWaitTime = TraverseExecuteTasks();

			if (m_mapTimerTask.empty()) // 如果没有任务了，就退出
			{
				break;
			}
		}
	}

	unsigned __int64 CTimerBase::TraverseExecuteTasks()
	{
		unsigned __int64 nMinWaitTime = 0xffffffffffffffff; // 所有任务中最小的等待时间，用于计算下一次执行需要多久
		__int64 hadWaitTime = 0; // 一个任务已经等待了的时间
		__int64 nNeedWaitTime = 0; // 一个任务还需要等待的时间

		unsigned __int64 nNowTime = GetNowTime();
		for (std::map<unsigned int, STimerTaskInter>::iterator it = m_mapTimerTask.begin(); it != m_mapTimerTask.end();)
		{
			STimerTaskInter& rTimerTaskInter = it->second;
			hadWaitTime = nNowTime - rTimerTaskInter.m_nStartTime; // 已经等待了的时间
			if (hadWaitTime < 0) // 不能小于0
			{
				hadWaitTime = 0;
			}

			if ((hadWaitTime + m_nLeadTime) >= rTimerTaskInter.m_timerTask.m_nInterval) // 时间间隔大于指定间隔时执行 可以提前m_nLeadTime毫秒执行
			{
				ExecuteTask(rTimerTaskInter.m_timerTask);
				rTimerTaskInter.m_nStartTime = nNowTime;
				if (!rTimerTaskInter.m_timerTask.m_bLoop) // 如果定时器只执行一次，则移除
				{
					it = m_mapTimerTask.erase(it);
					continue;
				}

				nNeedWaitTime = rTimerTaskInter.m_timerTask.m_nInterval; // 一个任务还需要等待的时间
			}
			else
			{
				nNeedWaitTime = rTimerTaskInter.m_timerTask.m_nInterval - hadWaitTime; // 一个任务还需要等待的时间
			}
			assert(nNeedWaitTime > 0); // 按照逻辑，nNeedWaitTime必然大于0

			// 找到最小的需要等待的时间
			if ((unsigned __int64)nNeedWaitTime < nMinWaitTime)
			{
				nMinWaitTime = nNeedWaitTime;
			}

			++it;
		}

		assert(nMinWaitTime > 0); // nMinWaitTime必然大于0
		return nMinWaitTime;
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
