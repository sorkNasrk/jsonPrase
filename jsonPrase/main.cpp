// jsonPrase.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "jsonPrase.h"

using namespace std;

int main(int argc, char**argv)
{
	struct stat op;
	if (argc < 2) {
		printf("please input filename\n");
		return 0;
	}
	printf("filename:%s\n", argv[1]);
	char * filename = argv[1];
	int rst = stat(argv[1], &op);
	if (rst < 0) {
		perror("get file size error");
	}
	off_t filesize = op.st_size;

	printf("file size:%lld\n", (long long)filesize);

	char *buf = new char[filesize + 1]();
	string sdd;
	sdd.c_str();
	
	ifstream file(filename);
	file.read(buf, filesize);

	cout << buf << endl;
	jsonNode * node = new jsonNode;
	parse_value(node, buf);
	print_json(node);
	system("pause");
    return 0;
}

