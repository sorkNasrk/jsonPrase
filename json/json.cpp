#include "stdafx.h"
#include "json.h"
#include <atomic>
#include <stdlib.h>
#include <new>
#include <iostream>

using namespace std;

//std::atomic_int jsonNode::totolSize = 0;
//
//void * jsonNode::operator new(size_t size) {
//	void *	ptr = malloc(size);
//	if (ptr) {
//		totolSize += size;
//
//		cout << "malloc:" << size << "totol:" << totolSize << endl;
//		return ptr;
//	}
//	else {
//		throw std::bad_alloc();
//	}
//}
//void * jsonNode::operator new[](size_t size) {
//	return operator new(size);
//}
//void jsonNode::operator delete(void * ptr, size_t size) {
//	totolSize -= size;
//	cout << "free:" << size << "totol:" << totolSize << endl;
//	free(ptr);
//}
//void jsonNode::operator delete[](void * ptr, size_t size) {
//	return operator delete(ptr, size);
//}

static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
json::json(const char * str){
	parse(str);
}

jsonNode * json::parse(const char * str)
{
	if (data)
	{
		delete data;
		
	}
	data = new jsonNode;
	if (str) {
		try
		{
			parse_value(data, str);
		}
		catch (const std::runtime_error& e)
		{
			delete data;
			data = nullptr;
			cout << e.what() << endl;
		}
		
	}
	return data;
}



const char * json::skip(const char * str) {
	if(!str)throw runtime_error("str is null");
	while (*str && (unsigned char)*str <= 32)
	{
		str++;
	}
	return str;
}

const char * json::parse_object(jsonNode * node, const char * str) {
	if (*str == '{') {
		str = skip(str + 1);
		while (*str && *str == '\"') {
			jsonNode * child = new jsonNode;
			string childname;

			try
			{
				str = parse_string(childname, str);
				str = skip(str);
				if (*str == ':') {
					str = parse_value(child, str + 1);
					node->children[childname] = child;
				}
			}
			catch (...)
			{
				delete child;
				throw;
			}


			str = skip(str);
			if (*str != ',')
				break;
			str = skip(str + 1);
		}
		str = skip(str);
		if (*str != '}') {
			throw runtime_error("invalid json");
			return 0;
		}
		else
		{
			return str + 1;
		}
	}
	else {
		return 0;
	}


}

const char * json::parse_array(jsonNode * node, const char * str) {
	if (str) {
		str = skip(str);
		if (str && *str++ == '[') {
			str = skip(str);
			while (str && *str != ']')
			{
				jsonNode * child = new jsonNode;
				
				try
				{
					str = parse_value(child, str);
				}
				catch (...)
				{
					delete child;
					throw;
				}

				
				node->array.push_back(child);
				str = skip(str);
				if (*str != ',')
					break;
				str = skip(str + 1);
			}
			if (str && *str++ == ']') {
				return str;
			}
		}

	}
	throw runtime_error("invalid json");
	return 0;
}

const char * json::parse_number(jsonNode * node, const char * str) {
	if (str) {
		int sign = 1;
		int subscale = 0;
		int signsubscale = 1;
		node->jsonNumber = 0;
		if (*str == '-')sign = -1, str++;
		if (*str >= '0' && *str <= '9') {
			while (*str >= '0' && *str <= '9')
			{
				node->jsonNumber = node->jsonNumber * 10 + *str - '0';
				str++;
			}
		}
		else {
			throw runtime_error("invalid json");
			return 0;
		}
		if (*str == '.') {
			int o = -1;
			str++;
			while (*str >= '0' && *str <= '9')
			{
				node->jsonNumber = node->jsonNumber + (*str - '0') * pow(10, o);
				o -= 1;
				str++;
			}
		}
		if (*str == 'e' || *str == 'E') {
			str++;

			if (*str == '+') str++;	else if (*str == '-') signsubscale = -1, str++;		/* With sign? */
			while (*str >= '0' && *str <= '9') subscale = (subscale * 10) + (*str++ - '0');	/* Number? */

		}

		node->jsonNumber = node->jsonNumber * pow(10, signsubscale * subscale);

		return str;


	}
	return 0;
}

const char * json::parse_value(jsonNode * node, const char * str) {
	if (!str) throw runtime_error("str is nullptr");
	str = skip(str);
	if (str && *str) {
		if (!str)						return 0;	/* Fail on null. */
		if (!strncmp(str, "null", 4)) { node->type = jsonNode::JSON_NULL;  return str + 4; }
		if (!strncmp(str, "false", 5)) { node->type = jsonNode::JSON_BOOL; node->jsonBool = false; return str + 5; }
		if (!strncmp(str, "true", 4)) { node->type = jsonNode::JSON_BOOL; node->jsonBool = true;	return str + 4; }
		if (*str == '\"') { node->type = jsonNode::JSON_STRING; return parse_string(node->jsonString, str); }
		if (*str == '-' || (*str >= '0' && *str <= '9')) { node->type = jsonNode::JSON_NUMBER; return parse_number(node, str); }
		if (*str == '[') { node->type = jsonNode::JSON_ARRAY; return parse_array(node, str); }
		if (*str == '{') { node->type = jsonNode::JSON_OBJECT; return parse_object(node, str); }

		throw runtime_error("invalid json");
	}

	return 0;
}

const char * json::parse_string(string & dst, const char * str) {
	if (*str == '\"') {
		const char *p = str + 1;
		int len = 0;

		unsigned int uc, uc2;

		while (*p != '\"' && *p) {
			if (*p++ == '\\') {
				p++;
			}
			len++;
		}

		char * strinfo = new char[len + 1]();
		
		try
		{
			char *sstr = strinfo;

			len = 0;
			p = str + 1;

			while (*p && *p != '\"') {
				if (*p == '\\') {
					p++;
					switch (*p)
					{
					case 'b':
						*sstr++ = '\b';
						break;
					case 'f':
						*sstr++ = '\f';
						break;
					case 'n':
						*sstr++ = '\n';
						break;
					case 'r':
						*sstr++ = '\r';
						break;
					case 't':
						*sstr++ = '\t';
						break;
					case 'u': /* transcode utf16 to utf8. */
						sscanf(p + 1, "%4x", &uc);
						p += 4; /* get the unicode char. */

						if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) { throw runtime_error("invalid json"); break; } /* check for invalid.	*/

																													   /* TODO provide an option to ignore surrogates, use unicode replacement character? */
						if (uc >= 0xD800 && uc <= 0xDBFF) /* UTF16 surrogate pairs.	*/
						{
							if (p[1] != '\\' || p[2] != 'u') { throw runtime_error("invalid json"); break; } /* missing second-half of surrogate.	*/
							sscanf(p + 3, "%4x", &uc2);
							p += 6;
							if (uc2 < 0xDC00 || uc2 > 0xDFFF) { throw runtime_error("invalid json"); break; } /* invalid second-half of surrogate.	*/
							uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
						}

						len = 4;
						if (uc < 0x80)
							len = 1;
						else if (uc < 0x800)
							len = 2;
						else if (uc < 0x10000) len = 3;
						sstr += len;

						switch (len) {
						case 4:
							*--sstr = ((uc | 0x80) & 0xBF);
							uc >>= 6;
							/* fallthrough */
						case 3:
							*--sstr = ((uc | 0x80) & 0xBF);
							uc >>= 6;
							/* fallthrough */
						case 2:
							*--sstr = ((uc | 0x80) & 0xBF);
							uc >>= 6;
							/* fallthrough */
						case 1:
							*--sstr = (uc | firstByteMark[len]);
						}
						sstr += len;
						break;
					default:
						*sstr++ = *p;
						break;
					}
				}
				else
				{
					*sstr++ = *p;
				}
				p++;
				len++;
			}
			*sstr = 0;
			dst = strinfo;
		}
		catch (...)
		{
			delete[] strinfo;
			throw;
		}


		delete[] strinfo;
		if (*p == '\"') return ++p;
		throw runtime_error("invalid json");
	}
	return 0;
}

json::~json()
{
	delete data;
}

jsonNode::~jsonNode()
{
	switch (type)
	{
	case jsonNode::JSON_ARRAY:
		for (auto i : array) {
			delete i;
		}
		break;
	case jsonNode::JSON_BOOL:
		break;
	case jsonNode::JSON_NULL:
		break;
	case jsonNode::JSON_NUMBER:
		break;
	case jsonNode::JSON_OBJECT:
		for (auto i : children) {
			delete i.second;
		}
		break;
	case jsonNode::JSON_STRING:
		
		break;
	default:
		break;
	}
}


void print_json(jsonNode * node, int space)
{
	if (node) {
		string block(space, '\t');
		switch (node->type)
		{
		case jsonNode::JSON_ARRAY:
			cout<< "[" << endl;
			for (auto i : node->array) {
				cout << block << "\t";
				print_json(i, space + 1);
				cout << "," << endl;
			}
			cout << block << "]";
			break;
		case jsonNode::JSON_BOOL:
			cout <<  (node->jsonBool ? "true" : "false");
			break;
		case jsonNode::JSON_NULL:
			cout << "null";
			break;
		case jsonNode::JSON_NUMBER:
			cout <<  node->jsonNumber;
			break;
		case jsonNode::JSON_OBJECT:
			cout << "{" << endl;
			for (auto i : node->children) {
				cout << block << "\t" << "\"" << i.first << "\"" << ":";
				print_json(i.second, space + 1);
				cout << "," << endl;
			}
			cout << block << "}";
			break;
		case jsonNode::JSON_STRING:
			cout << "\"" << node->jsonString << "\"";
			break;
		default:
			break;
		}
	}
}