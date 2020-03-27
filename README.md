# 性能测试课作业：基于采样的方法实现一个函数热点分析工具

## [思路](https://blog.csdn.net/u013431916/article/details/79908604)：
* 中断
* 查函数名i
* 计数器H[i]++ 
* 统计每个函数采样次数和占比

## 实现
1. 利用itimer计数器（定时器alarm时间间隔1秒太大了）
2. 利用signal接收SIGALRM信号
3. 在handler中进行采样，利用[libunwind](https://www.nongnu.org/libunwind/docs.html)库，找到函数名称  
安装我是直接sudo apt-get install libunwind-dev 就好了。  
[例程1](https://www.jianshu.com/p/f19cbeeafd45) 和[例程2](https://github.com/daniel-thompson/libunwind-examples)
4. 统计占比输出

## 结果
<img src="https://github.com/xinxin1000/Sampling-Func-Count/blob/master/freq-profile-test.png" width="600" height="150" align="bottom" />
