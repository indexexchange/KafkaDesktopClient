#include "lexer.h"

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <memory>

#include "filter.h"
#include "../logger.h"
#include "tokenizer.h"
#include "jqutil.h"

namespace jq::internal {

	vector<filter_ptr> parse(string query) {
		Tokenizer tokenizer(query);
		//tokenizer.Print();
		auto tokens = tokenizer.Get();

		vector<filter_ptr> filter_chain;

		// Build Filter filter_chain_
		for(size_t p1=0; p1< tokens.size(); ++p1) {
			size_t p2 = p1 + 1;

			auto currentToken = tokens[p1];
			std::string nextToken;
			if (p2 < tokens.size()) {
				nextToken = tokens[p2];
			}

			//logger_info << currentToken << "," << nextToken;

			// Eliminate some base cases, eg ".." or ".|"
			if (currentToken == "." && nextToken == ".") {
				// skip
				continue;
			}
			else if (currentToken == "." && nextToken == "|") {
				// skip
				continue;
			}
			else if (currentToken == "|") {
				// skip
				continue;
			}
			else if (currentToken == "." && nextToken == "[") {
				// skip
				continue;
			}

			auto nextSection = getNextSection(tokens, p1);
			//logger_info << "nextSection: ";
			//print(tokens, nextSection.low, nextSection.high);

			if (nextSection.type == SectionType::FIELD_ACCESS) {
				
				auto low = nextSection.low;
				auto high = nextSection.high;	
				auto filters = buildFieldAccessFilter(tokens, low, high);

				for (auto& f : filters) {
					filter_chain.push_back(f);
					//logger_info << f->ToString();
				}

				p2 = high;
			}
			else if (nextSection.type == SectionType::SELECT) {
				// Detected this section to be a Select
				auto low = nextSection.low;
				auto high = nextSection.high;

				filter_ptr select_filter = buildSelectFilter(tokens, low, high);
				filter_chain.push_back(select_filter);
				p2 = high;
			}
			else if (nextSection.type == SectionType::REGEX) {
				// Detected this section to be a Regex function
				auto low = nextSection.low;
				auto high = nextSection.high;

				filter_ptr regex_filter = std::make_shared<Regex>(tokens, low, high);
				filter_chain.push_back(regex_filter);
				p2 = high;
			}
			else if (nextSection.type == SectionType::CONTAINS) {
				// Detected this section to be a Regex function
				auto low = nextSection.low;
				auto high = nextSection.high;

				filter_ptr contains_filter = std::make_shared<Contains>(tokens, low, high);
				filter_chain.push_back(contains_filter);
				p2 = high;
			}
			else if (nextSection.type == SectionType::BOOL_OP) {
				size_t low = nextSection.low;
				size_t high = nextSection.high;
				//print(tokens, low, high);

				auto filter = make_shared<Generic>();
				filter->bool_operation = buildBoolOp(tokens,low,high);
				filter_chain.push_back(filter);
				p2 = high;
			}
			else {
				logger_err << "Error: parsing. currentToken:" << currentToken << ", nextToken:" << nextToken << "," << endl;
			}

			p1 = p2;
		}

		return filter_chain;
	}

	std::pair<size_t, size_t> strip_paranthesis(std::vector<std::string>& tokens, size_t low, size_t high) {

		if (tokens[low] == "select") {
			++low;
		}

		while (tokens[low] == "(" && find_corresponding_match(tokens, low, high, "(", ")") == high) {
			++low;
			--high;
		}

		return std::make_pair(low, high);
	}

	size_t find_root(std::vector<std::string>& tokens, size_t low, size_t high) {

		auto p = strip_paranthesis(tokens, low, high);
		low = p.first;
		high = p.second;

		//logger_info << "find_root in following tokens:";
		//print(tokens, low, high);

		int paranth_count = 0;
		// Check for OR operations
		for (auto i = low; i <= high; ++i) {

			//logger_info << tokens[i];
			if (tokens[i] == "(") {
				++paranth_count;
			}

			if (tokens[i] == ")") {
				--paranth_count;
			}

			if ((tokens[i] == "||" || tokens[i] == "or") && paranth_count==0) {
				//this is an Or root
				return i;
			}
		}

		// Check for AND operations
		for (auto i = low; i <= high; ++i) {
			if (tokens[i] == "(") {
				++paranth_count;
			}

			if (tokens[i] == ")") {
				--paranth_count;
			}

			if ((tokens[i] == "&&" || tokens[i] == "and") && paranth_count==0) {
				//this is an And root
				return i;
			}
		}

		// Check for pipe here
		for (auto i = low; i <= high; ++i) {
			if (tokens[i] == "(") {
				++paranth_count;
			}
			if (tokens[i] == ")") {
				--paranth_count;
			}
			if (tokens[i] == "|" && paranth_count == 0) {
				return i;
			}
		}

		// Check for COMPARISON operations (should be compare_v operations)
		for (auto i = low; i <= high; ++i) {
			if (tokens[i] == "(") {
				++paranth_count;
			}

			if (tokens[i] == ")") {
				--paranth_count;
			}

			if (paranth_count==0 && (
				tokens[i] == "==" ||
				tokens[i] == "!=" ||
				tokens[i] == ">" ||
				tokens[i] == ">=" ||
				tokens[i] == "<" ||
				tokens[i] == "<="
				)) {
				//this is an == root
				return i;
			}
		}

		if (tokens[low] == "contains") {
			return low;
		}

		if (tokens[low] == "regex" || tokens[low] == "test") {
			return low;
		}

		if (tokens[low] == "not") {
			return low;
		}

		return low;
	}

	shared_ptr<Filter> buildBoolOp(vector<string>& tokens, size_t low, size_t high) {
		// Build a single boolean operation

		auto p = strip_paranthesis(tokens, low, high);
		low = p.first;
		high = p.second;

		// Find root node
		size_t root = find_root(tokens, low, high);
		//logger_info << "root is: " << tokens[root] << endl;
		//print(tokens, low, high);

		// Compare operation
		if (tokens[root] == "==" ||
			tokens[root] == "!=" ||
			tokens[root] == ">" ||
			tokens[root] == "<" ||
			tokens[root] == ">=" ||
			tokens[root] == "<="
			) {
			auto comparison_op = make_shared<Compare>(tokens, low, high);
			return comparison_op;

		}
		else if (tokens[root] == "||" || tokens[root] == "or") {
			auto or_op = make_shared<Or>();
			auto left_op = buildBoolOp(tokens, low, root - 1);
			auto right_op = buildBoolOp(tokens, root + 1, high);
			or_op->children.push_back(left_op);
			or_op->children.push_back(right_op);
			return or_op;
		}
		else if (tokens[root] == "&&" || tokens[root] == "and") {
			auto and_op = make_shared<And>();
			auto left_op = buildBoolOp(tokens, low, root - 1);
			auto right_op = buildBoolOp(tokens, root + 1, high);
			and_op->children.push_back(left_op);
			and_op->children.push_back(right_op);
			return and_op;
		}
		else if (tokens[root] == "contains") {

			if (low != root) {
				logger_err << "Could not build contains(.) operation";
			}

			auto contains_op = make_shared<Contains>(tokens, low, high);
			return contains_op;
		}
		else if (tokens[root] == "regex" || tokens[root] == "test") {
			if (low != root) {
				logger_err << "Could not build regex(.) operation";
			}

			//logger_info << tokens[low];
			auto regex_op = make_shared<Regex>(tokens, low, high);
			return regex_op;
		}
		else if (tokens[root] == "not") {
			// NOT operation
			auto not_op = make_shared<Not>(tokens, low, high);
			return not_op;
		}
		else if (tokens[root] == "." && find_next_match(tokens,low,high,"|")!= low) {
			// .some.value | contains() /regex() / not
			auto op = std::make_shared<Generic>();

			auto pipe_pos = find_next_match(tokens, low, high, "|");
			auto filters = buildFieldAccessFilter(tokens, low, pipe_pos-1);

			for (auto f : filters) {
				op->children.push_back(f);
			}

			if (tokens[pipe_pos + 1] == "contains") {
				auto contains = std::make_shared<Contains>(tokens, pipe_pos + 1, pipe_pos + 4);
				op->children.push_back(contains);
			}
			else if (tokens[pipe_pos + 1] == "regex") {
				auto regex = std::make_shared<Regex>(tokens, pipe_pos + 1, pipe_pos + 4);
				op->children.push_back(regex);
			}
			else if (tokens[pipe_pos + 1] == "not") {
				auto not_op = std::make_shared<Not>(tokens, pipe_pos+1, pipe_pos+1);
				op->children.push_back(not_op);
			}
			else {
				throw JQException(JQEXCEPT_MALFORMED_QUERY);
			}

			return op;
		}
		else if (tokens[root] == "|") {
			// eg. select ( key == "value" | not )
			auto op = std::make_shared<Generic>();
			op->children.push_back(buildBoolOp(tokens,low, root-1));
			op->children.push_back(buildBoolOp(tokens,root+1, high));
			return op;
		}
		else {
			// select ( .some.resulting.value )
			auto exists_op = make_shared<Exists>(tokens, low, high);
			return exists_op;
		}

		return shared_ptr<Filter>();
	}

	std::shared_ptr<Filter> buildSelectFilter(std::vector<std::string>& tokens, size_t low, size_t high) {
		for (auto& t : tokens) {
			//cout << t << " ";
		}
		//cout << endl;

		if (tokens[low] != "select" && tokens[low+1] != "(" && tokens[high] != ")") {
			logger_err << "ERROR: throw, select filter in wrong format, must use: select()\n";
		}

		auto p = strip_paranthesis(tokens,low,high);
		low = p.first;
		high = p.second;

		//logger_info << "build select with following tokens: ";
		//print(tokens, low, high);

		// Create a BOP (which has children and eval())
		auto select = make_shared<Select>();

		std::vector<filter_ptr> filters;
		
		select->bool_operation.push_back(buildBoolOp(tokens,low,high));

		//logger_info << "Built Select Statement: " << select->ToString() << endl;

		return select;
	}

	std::vector<filter_ptr> buildFieldAccessFilter(std::vector<std::string>& tokens, size_t low, size_t high)
	{
		std::vector<filter_ptr> filters;

		for (auto i = low; i <= high; ++i) {

			if (i >= tokens.size()) {
				break;
			}

			if (tokens[i] == "." && tokens[i+1] == "\"") {
				auto filter = make_shared<ObjectIdentifierIndex>(tokens, i, i+3);
				filters.push_back(filter);
				i = i + 3;
			}
			else if (tokens[i] == "." && tokens[i+1] == "[") {
				filter_ptr filter = make_shared<ArrayIndex>(tokens, i+1, i + 3);
				filters.push_back(filter);
				i = i + 3;
			}
			else if (tokens[i] == "[") {
				filter_ptr filter = make_shared<ArrayIndex>(tokens, i, i + 2);
				filters.push_back(filter);
				i = i + 2;
			}
			else if (tokens[i] == ".") {
				auto filter = make_shared<ObjectIdentifierIndex>(tokens, i, i + 1);
				filters.push_back(filter);
				i = i + 1;
			}
			else {
				logger_err << "wrong state, i:" << i << ", token: " << tokens[i];
			}
		}
		
		return filters;
	}

	Section getNextSection(std::vector<std::string>& tokens, size_t low)
	{
		
		// SELECT 
		if (tokens[low] == "select" && low + 1 < tokens.size() && tokens[low + 1] == "(") {
			// Detected this section to be a Select
			Section section;
			size_t high = low;
			int paranth_count = 1;
			high = low + 2;

			while (paranth_count != 0) {
				auto tok = tokens[high++];
				if (tok == ")") {
					--paranth_count;
				}
				else if (tok == "(") {
					++paranth_count;
				}

				if (paranth_count == 0) {
					break;
				}
			}
			section.low = low;
			section.high = high-1;
			section.type = SectionType::SELECT;
			return section;
		}
		// REGEX 
		else if(tokens[low] == "regex" && low + 1 < tokens.size() && tokens[low + 1] == "("){
			// Detected this section to be a Select
			Section section;
			size_t high = low;
			int paranth_count = 1;
			high = low + 2;

			while (paranth_count != 0) {
				auto tok = tokens[high++];
				if (tok == ")") {
					--paranth_count;
				}
				else if (tok == "(") {
					++paranth_count;
				}

				if (paranth_count == 0) {
					break;
				}
			}
			section.low = low;
			section.high = high - 1;
			section.type = SectionType::REGEX;
			return section;
		}
		// CONTAINS 
		else if (tokens[low] == "contains" && low + 1 < tokens.size() && tokens[low + 1] == "(") {
			// Detected this section to be a Select
			Section section;
			size_t high = low;
			int paranth_count = 1;
			high = low + 2;

			while (paranth_count != 0) {
				auto tok = tokens[high++];
				if (tok == ")") {
					--paranth_count;
				}
				else if (tok == "(") {
					++paranth_count;
				}

				if (paranth_count == 0) {
					break;
				}
			}
			section.low = low;
			section.high = high - 1;
			section.type = SectionType::CONTAINS;
			return section;
		}
		// FIELD ACCESS
		else if (tokens[low] == ".") {
			Section section;
			section.type = SectionType::NONE;
			size_t high = low;

			while (high < tokens.size() && tokens[high] != "|") {
				if (BOOL_OPS.find(tokens[high]) != BOOL_OPS.end()) {
					//this is a bool operation
					section.type = SectionType::BOOL_OP;
				}
				++high;
			}
			
			section.low = low;
			section.high = high -1;
			if (section.type == SectionType::NONE) {
				section.type = SectionType::FIELD_ACCESS;
			}
			
			return section;
		}
		// [0] 
		else if (tokens[low] == "[" && low + 2 < tokens.size() && tokens[low+2] == "]") {
			// this section can be either an array access or array element comparison
			Section section;
			section.type = SectionType::NONE;
			size_t high = low + 2;
			section.low = low;
			

			while (high < tokens.size() && tokens[high] != "|") {
				if (BOOL_OPS.find(tokens[high]) != BOOL_OPS.end()) {
					//this is a bool operation
					section.type = SectionType::BOOL_OP;
				}
				++high;
			}

			section.high = high-1;
			if (section.type == SectionType::NONE) {
				section.type = SectionType::FIELD_ACCESS;
			}
			return section;
		}
		//
		else if (tokens[low] == "(" && tokens[tokens.size()-1] == ")" && std::count(tokens.begin(), tokens.end(), "(") == 1) {
			return getNextSection(tokens, low + 1);
		}
		// (_) bool_op _
		else if (tokens[low] == "(") {
			//advance past the bracket
			Section section;
			size_t high = low;
			int paranth_count = 0;
			while (high < tokens.size() && tokens[high] != "|") {
				if (tokens[high] == "(") {
					++paranth_count;
				}
				if (tokens[high] == ")") {
					--paranth_count;
				}

				if (paranth_count == 0 && BOOL_OPS.find(tokens[high]) != BOOL_OPS.end()) {
					section.type = SectionType::BOOL_OP;
				}
				++high;
			}

			section.low = low;
			section.high = high - 1;
			return section;
		}
		else if (tokens[low] == "not") {
			Section section;
			size_t high = low;
			section.low = low;
			section.high = high;
			section.type = SectionType::BOOL_OP;
			return section;
		}

		//logger_err << "Something is wrong with this query";
		throw JQException(JQEXCEPT_MALFORMED_QUERY);
	}

	inline bool containsFieldAccess(vector<string>& tokens, size_t low, size_t high) {
		for (auto i = low; i < high; ++i) {
			if (tokens[i] == "." || tokens[i] == "[" || tokens[i] == "]") {
				return true;
			}
		}
		return false;
	}
	inline bool containsString(vector<string>& tokens, size_t low, size_t high) {
		for (auto i = low; i < high; ++i) {
			if (tokens[i] == "\"") {
				return true;
			}
		}
		return false;
	}
	bool isStopKeyword(std::string)
	{
		return false;
	}
}

