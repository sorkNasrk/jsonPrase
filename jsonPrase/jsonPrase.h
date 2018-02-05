#pragma once
#include <string>
#include <vector>
#include <map>
class jsonNode
{
public:
	enum NODETYPE
	{
		JSON_NUMBER,
		JSON_STRING,
		JSON_BOOL,
		JSON_OBJECT,
		JSON_ARRAY,
		JSON_NULL,
	};

	NODETYPE type;

	long double jsonNumber;
	std::string jsonString;
	bool jsonBool;
	std::vector<jsonNode *> array;

	std::string name;
	
	std::map<std::string, jsonNode*> children;
};
static const char * skip(const char * str);

const char * parse_object(jsonNode * node, const char * str);
const char * parse_array(jsonNode * node, const char * str);
const char * parse_number(jsonNode * node, const char * str);
const char * parse_value(jsonNode * node, const char * str);
const char * parse_string(std::string & dst, const char * str);

void print_json(jsonNode * node);