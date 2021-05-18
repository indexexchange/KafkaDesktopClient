#include "tokenizer.h"

#include <deque>
#include <functional> 
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "common.h"
#include "../logger.h"
#include "jqutil.h"

namespace jq {
	namespace internal {
		Tokenizer::Tokenizer(std::string q) : query(q)
		{
			// If last character is a newline, it means user copy-pasted this from some site... don't throw exception, just clean it up
			if (*query.rbegin() == '\n') {
				query.pop_back();
			}

			//fill tokens
			for (size_t p1 = 0; p1 < query.size(); ++p1) {
				for (size_t p2 = p1; p2 <= query.size(); ++p2) {

					std::string partial = query.substr(p1, p2 - p1);

					if (partial == " ") {
						break;
					}

					// detect start of regex expr
					if ((partial == "regex" || partial == "REGEX" || 
						 partial == "test" || partial == "TEST" ||
						 partial == "contains" || partial == "CONTAINS") && query[p2] == '(') {

						push_back(partial);	// start pushing regex(...
						push_back("(");

						++p2;
						p1 = p2;	// bring p1 to first char in regex expression

						size_t parant_cnt = 1;

						while (parant_cnt != 0 && p2 <= query.size()) {
							if (query[p2] == '(') {
								++parant_cnt;
							}
							else if (query[p2] == ')') {
								--parant_cnt;
								break;
							}
							++p2;

							if (p2 == query.size()) {
								throw JQException(JQEXCEPT_NON_MATCH_PARANTH);
							}
						}

						if (parant_cnt != 0) {
							logger_err << "error parsing regex function, no match parantheses";
						}

						partial = query.substr(p1, p2 - p1);
						push_back(partial);
						push_back(")");
						p1 = p2;
						break;
					}

					// Detect new space => this means the token has finished
					if (!partial.empty() && query[p2] == ' ') {
						push_back(partial);
						p1 = p2;
						break;
					}

					// detect open/close quotes
					if (query[p2] == '\"') {

						if (contains_keyword(partial)) {
							// special case like some_key==" <- need to split "some_key" and "==" first
							auto tok = split(partial);
							for (auto t : tok) {
								push_back(t);
							}
							p1 = p2;
						}
						else if (!partial.empty()) {
							logger_err << "issue with open/close quotes";
							push_back(partial);
							partial.clear();
							p1 = p2;
						}
						push_back("\"");
						++p2;

						while (query[p2] != '\"') { // look for matching closing quote
							if (p2 == query.size()) {
								throw JQException(JQEXCEPT_NON_MATCH_QUOTES);
							}
							++p2;
						}

						// guaranteed to have found matching close quotes by this point
						partial = query.substr(p1 + 1, p2 - p1 - 1);
						push_back(partial);
						push_back("\"");

						p1 = p2;
						break;
					}

					// detect open/close parantheses
					if (query[p2] == '(' || query[p2] == ')') {

						if (!partial.empty()) {
							push_back(partial);
						}

						std::string paranth(1, query[p2]);
						push_back(paranth);
						p1 = p2;
						break;
					}

					// check if we're parsing a number like 1.475
					if (is_int(partial)) {
						while (isdigit(query[p2]) || query[p2] == '.') {
							partial += query[p2];
							++p2;
						}
						push_back(partial);
						p1 = p2 - 1;
						break;
					}

					// detect dot
					if (query[p2] == '.') {
						if (partial == "." || partial.empty()) {
							push_back(".");
						}
						else if(!partial.empty()) {
							// detected a dot but with another string before it ( request. ) - break them
							push_back(partial.substr(0, partial.size()));
							push_back(".");
						}

						p1 = p2;
						break;
					}

					for (const auto& k : keywords) {
						auto pos = partial.find(k);

						if (pos != std::string::npos) {
							//logger_info << "partial:" << partial << ", key: " << k;

							if (partial == "|" && p2 != query.size() && query[p2] == '|') {
								// Detect Or (make sure we don't confuse with single pipe |)
								++p2;
								p1 = p2;
								push_back("||");
							}
							else if (partial == ">" && p2 != query.size() && query[p2] == '=') {
								// Detect >=
								++p2;
								p1 = p2;
								push_back(">=");
							}
							else if (partial == "<" && p2 != query.size() && query[p2] == '=') {
								// Detect <=
								++p2;
								p1 = p2;
								push_back("<=");
							}
							else if (pos == 0) {
								// the whole partial is a keyword
								push_back(partial);
								p1 = p2;
							}
							else {

								if ((k == "and" || k == "or" || k == "not" ||
									k == "AND" || k == "OR" || k == "NOT" ) &&
									partial[pos-1] != ' ') {
									//continue
								}
								else {
									// need to split from begin->pos-1 and pos->end
									push_back(partial.substr(0, pos));
									push_back(partial.substr(pos, partial.size() - pos));
									p1 = p2;
								}
							}
						}
					}

					if (p2 == query.size() && p1 != query.size() && !partial.empty()) {
						push_back(partial);
						p1 = p2;
					}
				}
			}

			// trim empty spaces from right_ side
			for (auto& tok : tokens) {
				tok = rtrim(tok);
			}

			self_validate();
		}

		Tokenizer::~Tokenizer()
		{
			tokens.clear();
		}

		std::string Tokenizer::Peek()
		{
			if (!tokens.empty()) {
				return tokens.front();
			}
			return "";
		}

		void Tokenizer::Pop()
		{
			tokens.erase(tokens.begin());
		}

		void Tokenizer::push_back(std::string token)
		{
			//logger_info << "push_back token: " << token;
			tokens.push_back(token);
		}

		bool Tokenizer::contains_keyword(std::string partial)
		{
			if (partial.empty()) {
				return false;
			}

			for (const auto& k : keywords) {
				auto pos = partial.find(k);

				if (pos != std::string::npos) {
					return true;
				}
			}

			return false;
		}

		std::vector<std::string> Tokenizer::split(std::string partial)
		{
			std::vector<std::string> result;
			for (const auto& k : keywords) {
				auto pos = partial.find(k);

				if (pos != std::string::npos) {
					// need to split from begin->pos-1 and pos->end
					result.push_back(partial.substr(0, pos));
					result.push_back(partial.substr(pos, partial.size() - pos));
				}
			}

			return result;
		}

		bool Tokenizer::HasNext()
		{
			return !tokens.empty();
		}

		bool Tokenizer::IsKeyword(std::string key)
		{
			return keywords.find(key) != keywords.end();
		}

		size_t Tokenizer::Size()
		{
			return tokens.size();
		}

		void Tokenizer::Print()
		{
			for (size_t i = 0; i < tokens.size(); ++i) {
				std::cout << "tokens["<< i << "]: " <<tokens[i] << std::endl;
			}
			std::cout << std::endl;
		}

		const std::vector<std::string>& Tokenizer::Get()
		{
			return tokens;
		}

		void Tokenizer::self_validate()
		{
			// 
			for (auto& token : tokens) {
				if (token == "AND")		token = "and";
				if (token == "OR")		token = "or";
				if (token == "CONTAINS")token = "contains";
				if (token == "REGEX")	token = "regex";
				if (token == "test")	token = "regex";
				if (token == "TEST")	token = "regex";
				if (token == "SELECT")	token = "select";
				if (token == "NOT")		token = "not";
			}

			// check for matching parantheses
			size_t count = 0;
			for (auto& t : tokens) {
				if (t == "(") {
					++count;
				}
				else if (t == ")") {
					--count;
				}
			}
			if (count != 0) {
				throw JQException(JQEXCEPT_NON_MATCH_PARANTH);
			}

			// check for mathcing square brackets
			for (size_t i = 0; i < tokens.size(); ++i) {
				if (tokens[i] == "[") {
					++count;
					if (i + 1 == tokens.size()) {
						throw JQException(JQEXCEPT_NON_MATCH_BRACKETS);
					}
					if (!is_int(tokens[i+1])) {
						throw JQException(JQEXCEPT_ARRAY_INDX_NOT_INT);
					}
				}
				else if (tokens[i] == "]") {
					--count;
				}
			}
			if (count != 0) {
				throw JQException(JQEXCEPT_NON_MATCH_BRACKETS);
			}

			//check for malformed query
			for (size_t i = 0; i < tokens.size(); ++i) {
				if (tokens[i] == "[" && tokens[i+2] != "]") {
					throw JQException(JQEXCEPT_MALFORMED_QUERY);
				}
				if (tokens[i] == "[" && !is_int(tokens[i+1])) {
					throw JQException(JQEXCEPT_ARRAY_INDX_NOT_INT);
				}

				// consecutive tokens test
				if (i + 1 < tokens.size()) {
					if (tokens[i] == "and" && (
						tokens[i+1] != "." &&
						tokens[i+1] != "(" &&
						tokens[i+1] != "contains" &&
						tokens[i=1] != "regex")) {
						throw JQException(JQEXCEPT_MALFORMED_QUERY);
					}
				}
				
			}


		}

	}
}
