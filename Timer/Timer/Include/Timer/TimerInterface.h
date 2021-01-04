#pragma once
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

// 2020-12-3 姜安富
// 定时器接口
// 一个定时器可以添加多个定时任务，对每个定时任务，可以只执行一次，也可以循环定时执行，当超过定时时间时执行
// 为了平衡效率，每个定时任务可以提前一定时间执行，因此可以设置提前量
// 定时最高能达到的精度为毫秒
#include <functional>

namespace jaf
{
	// 定时任务参数
	struct STimerTask
	{
		std::function<void(void)> m_fun; // 定时执行函数

		bool m_bLoop = false; // 是否循环执行 false只执行一次，true循环执行
		unsigned int m_nInterval = 1000; // 定时时间间隔（毫秒）
	};

	// 定时器接口
	class CTimerInterface
	{
	public:
		CTimerInterface() {};
		virtual ~CTimerInterface() {};

		// 添加定时任务
		// nTimeId 定时Id，不能为0
		// rTask 定时任务信息
		// 返回定时任务，返回0时表示创建定时任务失败
		virtual unsigned int AddTask(unsigned int nTimeId, const STimerTask& rTask) = 0;
		// 清除所有定时任务
		virtual void Clear() = 0;
		// 移除一个定时任务
		// nTimeId 要移除的定时任务的Id
		virtual void Remove(unsigned int nTimeId) = 0;

		// 设置定时的提前量，对于给定的任务，可以提前一定时间执行，单位毫秒
		virtual void SetLeadTime(unsigned int nLeadTime)
		{
			m_nLeadTime = nLeadTime;
		}

	protected:
		unsigned int m_nLeadTime = 20; // 执行任务的提前量，对于给定的任务可以提前m_nLeadTime毫秒执行	
	};
};