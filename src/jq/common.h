#ifndef COMMON_H_
#define COMMON_H_

#include <memory>
#include <vector>
#include <unordered_set>
#include <string>

#include "rapidjson/document.h"

#define string_ptr std::shared_ptr<std::string>

// Define Exceptions
#define JQEXCEPT_NON_MATCH_QUOTES		"non-matching quotes"
#define JQEXCEPT_NON_MATCH_PARANTH		"non-matching parantheses"
#define JQEXCEPT_NON_MATCH_BRACKETS		"non-matching square brackets"
#define JQEXCEPT_ARRAY_INDX_NOT_INT		"array index is not an integer"
#define JQEXCEPT_MALFORMED_QUERY		"malformed query"

using namespace rapidjson;

//TODO: consolidate these with defines
const std::unordered_set<std::string> keywords = {
		"select", "contains", "regex",
		"and", "or", "not", "||", "&&",
		".", "(", ")", "[", "]", "\"",
		"|",
		"==","!=", ">",">=","<","<=" };

#define EQUAL		"=="
#define NOT_EQ		"!="
#define SMALLER		"<"
#define SMALLER_EQ	"<="
#define LARGER		">"
#define LARGER_EQ	">="

const std::unordered_set<std::string> COMPARE_OPS = {
	EQUAL,
	NOT_EQ,
	SMALLER,
	SMALLER_EQ,
	LARGER,
	LARGER_EQ
};

const std::unordered_set<std::string> BOOL_OPS = {
	EQUAL,
	NOT_EQ,
	SMALLER,
	SMALLER_EQ,
	LARGER,
	LARGER_EQ,
	"and", "or" ,"||", "&&"
};

class JQException : public std::exception {
	std::string message;
public:
	JQException(std::string msg) : message(msg){}
	inline std::string what() { return message; }
};

#endif // COMMON_H_