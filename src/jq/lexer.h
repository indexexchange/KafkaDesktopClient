#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <stack>
#include <vector>
#include <memory>

#include "filter.h"

namespace jq::internal {
	enum class SectionType {NONE=0, SELECT, FIELD_ACCESS, BOOL_OP, REGEX, CONTAINS};

	struct Section {
		size_t low;
		size_t high;
		SectionType type;
	};
}


namespace jq::internal {

	std::vector<filter_ptr> parse(std::string query);

	size_t find_root(std::vector<std::string>&, size_t, size_t);

	std::shared_ptr<Filter> buildBoolOp(std::vector<std::string>&, size_t, size_t);

	filter_ptr buildSelectFilter(std::vector<std::string>&, size_t, size_t);

	std::vector<filter_ptr> buildFieldAccessFilter(std::vector<std::string>&, size_t, size_t);

	Section getNextSection(std::vector<std::string>&, size_t);

	inline bool containsFieldAccess(vector<string>&, size_t, size_t);
	inline bool containsString(vector<string>&, size_t, size_t);
	inline bool isStopKeyword(std::string);
}


#endif // LEXER_H_