#include <thread>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
void cmdThread(char * a) {
	std::cout << "thread " << *a<<std::endl;
}
int main0() {
	char* a = "cbc";
	std::thread t1(cmdThread,a);
	t1.join();
	return 0;
}