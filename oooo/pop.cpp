#include "stdafx.h"
#include <stdlib.h>
#include <new>
#include <iostream>
using namespace std;

void operator delete(void * ptr) {
	cout << "����delete" << endl;
	free(ptr);
	return;
}
//void operator delete(void * ptr, size_t size) {
//	cout << "���ô�size������delete" << endl;
//	free(ptr);
//	return;
//}
//void operator delete[](void * ptr) {
//	cout << "����delete[]" << endl;
//	free(ptr);
//	return;
//}
//void operator delete[](void * ptr, size_t size) {
//	cout << "���ô�size������delete[]" << endl;
//	free(ptr);
//	return;
//}
void* operator new(size_t size) {
	return malloc(size);
}
void* operator new[](size_t size) {
	return malloc(size);
}