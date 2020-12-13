﻿#include <iostream>
#include <sstream>
#include <functional>
#include "Timer/Head.h"

// 定时测试函数
void TimerTest(int n)
{
	std::cout << "TimerTest, n = " << n << std::endl;
}


// 定时测试类
class CTimerTest
{
public:
	// 成员变量作为定时执行的测试函数
	void TimerTest(int n)
	{
		std::cout << "CTimerTest::TimerTest, n = " << n << std::endl;
	}
};

int main()
{
	jaf::CHighPrecisionTimer timer; // 定义一个定时器

	int n = 1; // 测试用参数

	jaf::STimerTask task; // 定时任务
	task.m_nInterval = 5000; // 定时5秒
	task.m_bLoop = false; // 不循环定时
	task.m_fun = std::bind(&TimerTest, n); // 用全局函数作为定时执行函数
	timer.AddTask(task); // 添加第一个定时任务

	task.m_nInterval = 1000; // 定时1秒
	task.m_bLoop = true; // 循环定时
	CTimerTest timerTest;
	task.m_fun = std::bind(&CTimerTest::TimerTest, &timerTest, n); // 用类的成员函数作为定时执行函数
	timer.AddTask(task); // 添加第2个定时任务

	std::cout << "输入任意字符停止定时" << std::endl;
	getchar();
	timer.Clear();

	system("pause");
	return 0;
}

