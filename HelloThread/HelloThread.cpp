// HelloThread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread>

#include <mutex>
#include<atomic>
#include<vector>
class A {
public:

	~A() {
		std::cout << "~A" << std::endl;
	}
};

std::mutex m;
std::atomic_int   a_int;
void thread_t() {
	for (int i = 0; i < 100; i++) {
		std::lock_guard<std::mutex> lg(m);
		//m.lock();
		std::cout << "thread_t " << i << std::endl;
		a_int++;
		//m.unlock();
	}
}

int main()
{
	std::vector<A*> va;
	for (int i = 0; i < 10; i++) {
		std::cout << " i = " << i << std::endl;
		A a;
		va.push_back(&a);// a对象会在这里调用析构函数 释放
	}

	std::cout << va.size() << std::endl;
	std::thread t(thread_t);
	std::thread t1(thread_t);
	t.join();
	t1.join();
	std::cout << "Hello World!\n";
	std::cout << "a_int = " <<a_int<< std::endl;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
