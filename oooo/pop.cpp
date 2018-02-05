#include "stdafx.h"
#include <stdlib.h>
#include <new>
#include <iostream>
using namespace std;

void operator delete(void * ptr) {
	cout << "调用delete" << endl;
	free(ptr);
	return;
}
//void operator delete(void * ptr, size_t size) {
//	cout << "调用带size参数的delete" << endl;
//	free(ptr);
//	return;
//}
//void operator delete[](void * ptr) {
//	cout << "调用delete[]" << endl;
//	free(ptr);
//	return;
//}
//void operator delete[](void * ptr, size_t size) {
//	cout << "调用带size参数的delete[]" << endl;
//	free(ptr);
//	return;
//}
void* operator new(size_t size) {
	return malloc(size);
}
void* operator new[](size_t size) {
	return malloc(size);
}