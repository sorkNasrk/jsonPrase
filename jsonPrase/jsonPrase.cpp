#include "stdafx.h"
#include "jsonPrase.h"
using namespace std;

static const char * skip(const char * str) {
	while (str && *str && (unsigned char)*str <=32)
	{
		str++;
	}
	return str;
}

const char * parse_object(jsonNode * node, const char * str) {
	if (str && *str == '{') {
		str = skip(str + 1);
		while (*str && *str == '\"') {
			jsonNode * child = new jsonNode;
			string childname;
			str = parse_string(childname, str);
			str = skip(str);
			if (*str == ':') {
				 str = parse_value(child, str+1);
				 node->children[childname] = child;
			}
			str = skip(str);
			if (*str != ',')
				break;
			str = skip(str + 1);
		}
		str = skip(str);
		if (*str != '}') {
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
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };


const char * parse_array(jsonNode * node, const char * str) {
	if (str) {
		str = skip(str);
		if (str && *str++ == '[') {
			str = skip(str);
			while (str && *str != ']')
			{
				jsonNode * child = new jsonNode;
				str = parse_value(child, str);
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
	return 0;
}

const char * parse_number(jsonNode * node, const char * str) {
	if (str) {
		int sign = 1;
		int subscale = 0;
		int signsubscale = 1;
		node->jsonNumber = 0;
		if (*str == '-')sign = -1,str++;
		if (*str >= '0' && *str <= '9') {
			while (*str >='0' && *str <= '9')
			{
				node->jsonNumber = node->jsonNumber * 10 + *str - '0';
				str++;
			}
		}
		else {
			return 0;
		}
		if (*str == '.') {
			int o = -1;
			str++;
			while (*str >= '0' && *str <= '9')
			{
				node->jsonNumber = node->jsonNumber + (*str - '0') * pow(10,o);
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

const char * parse_value(jsonNode * node, const char * str) {
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
	}

	return 0;
}

const char * parse_string( string & dst, const char * str) {
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

					if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break; /* check for invalid.	*/

																		  /* TODO provide an option to ignore surrogates, use unicode replacement character? */
					if (uc >= 0xD800 && uc <= 0xDBFF) /* UTF16 surrogate pairs.	*/
					{
						if (p[1] != '\\' || p[2] != 'u') break; /* missing second-half of surrogate.	*/
						sscanf(p + 3, "%4x", &uc2);
						p += 6;
						if (uc2 < 0xDC00 || uc2 > 0xDFFF) break; /* invalid second-half of surrogate.	*/
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
		delete[] strinfo;
		if (*p == '\"') return ++p;
	}
	return 0;
}

void print_json(jsonNode * node)
{
	if (node) {

		switch (node->type)
		{
		case jsonNode::JSON_ARRAY:
			printf("[\n");
			for (auto i : node->array) {
				printf(" ");
				print_json(i);
				printf(",\n");
			}
			printf("]");
			break;
		case jsonNode::JSON_BOOL:
			printf("%s", node->jsonBool ? "true" : "false");
			break;
		case jsonNode::JSON_NULL:
			printf("%s","NULL");
			break;
		case jsonNode::JSON_NUMBER:
			printf("%lf", node->jsonNumber);
			break;
		case jsonNode::JSON_OBJECT:
			printf("{\n");
			for (auto i : node->children) {
				printf(" ");
				printf("\"%s\":", i.first.c_str());
				print_json(i.second);
				printf("\n");
			}
			printf("}");
			break;
		case jsonNode::JSON_STRING:
			printf("\"%s\"", node->jsonString.c_str());
			break;
		default:
			break;
		}
	}
}

