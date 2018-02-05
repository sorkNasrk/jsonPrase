#pragma once
#include <string>
#include <vector>
#include <map>
#include <atomic>
class jsonNode
{
public:

	//static std::atomic_int totolSize;

	//void * operator new(size_t size);
	//void * operator new[](size_t size);
	//void operator delete(void * ptr, size_t size);
	//void operator delete[](void * ptr, size_t size);

	enum NODETYPE
	{
		JSON_NUMBER,
		JSON_STRING,
		JSON_BOOL,
		JSON_OBJECT,
		JSON_ARRAY,
		JSON_NULL,
	};

	NODETYPE type = JSON_NULL;

	long double jsonNumber;
	std::string jsonString;
	bool jsonBool;
	std::vector<jsonNode *> array;

	//std::string name;

	std::map<std::string, jsonNode*> children;
	virtual ~jsonNode();
};


class json final
{
public:
	json(const char * str);
	jsonNode * parse(const char * str);
	jsonNode * getNode() { return data; }
	json(const json &) = delete;
	json &operator=(const json &) = delete;
	~json();
private:
	static const char * skip(const char * str);
	const char * parse_object(jsonNode * node, const char * str);
	const char * parse_array(jsonNode * node, const char * str);
	const char * parse_number(jsonNode * node, const char * str);
	const char * parse_value(jsonNode * node, const char * str);
	const char * parse_string(std::string & dst, const char * str);

	bool isValid = false;
	jsonNode * data = nullptr;
};
void print_json(jsonNode * node, int space = 0);
