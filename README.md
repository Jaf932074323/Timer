﻿# Timer
定时器

仅在windows10平台下，vs2019编译器上，使用ISO C++14 标准编译通过，目前没有跨平台。
因为代码较少，没有生成库。直接拷贝除Main.cpp之外的.cpp和.h文件到自己的项目中使用。

Main.cpp文件中举例了使用方法。

每一个定时器用一个单独的线程来实现定时器的定时任务。
同一个定时器可以添加多个定时任务。但因为每个定时任务的执行需要时间，相互之间可能会有影响。
执行耗时的任务时，需要在定时的执行函数中建立单独的线程来执行定时任务。