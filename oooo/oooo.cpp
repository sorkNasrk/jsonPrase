// oooo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <new>
#include <iostream>
#include <exception>
using namespace std;

class jsonNode
{
public:
	jsonNode() {
		throw runtime_error("123123");
	}
	//void * operator new(size_t size);
	//void * operator new[](size_t size);
	//void operator delete(void * ptr);
	void operator delete(void * ptr, size_t size);
	void operator delete[](void * ptr, size_t size);
	//void operator delete[](void * ptr);
};
//void * jsonNode::operator new(size_t size) {
//	void *	ptr = malloc(size);
//	if (ptr) {return ptr;}
//	else { throw std::bad_alloc(); }
//}
//
//void * jsonNode::operator new[](size_t size) {
//	return operator new(size);
//}
//void jsonNode::operator delete(void * ptr) {
//	cout << "�������е�delete" << endl;
//	free(ptr);
//}
void jsonNode::operator delete(void * ptr, size_t size) {
	cout << "�������д�size������delete" << endl;
	free(ptr);
}
void jsonNode::operator delete[](void * ptr, size_t size) {
	cout << "�������д�size������delete[]" << endl;
	free(ptr);
}
//void jsonNode::operator delete[](void * ptr) {
//	cout << "�������е�delete[]" << endl;
//	free(ptr);
//}


int main()
{
	int *p = new int;
	delete p;
	int *p0 = new int[10];
	delete[] p0;
	try
	{
		jsonNode *p1 = new jsonNode[10];
		delete[] p1;

		jsonNode *p2 = new jsonNode;
		delete p2;
	}
	catch (...)
	{
		cout << "error throw" << endl;
	}
	cout << "123" << endl;
	system("pause");
    return 0;
}

