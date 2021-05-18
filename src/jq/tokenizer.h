#ifndef PARSER_H_
#define PARSER_H_

#include <deque>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace jq {
	namespace internal {
		class Tokenizer {
			std::string query;
			std::vector<std::string> tokens;

			void push_back(std::string);
			bool contains_keyword(std::string);
			std::vector<std::string> split(std::string);
			void self_validate();

		public:
			Tokenizer(std::string q);
			~Tokenizer();

			std::string Peek();
			void Pop();

			bool HasNext();
			size_t Size();

			bool IsKeyword(std::string key);
			void Print();

			const std::vector<std::string>& Get();
		};
	}
}



#endif // PARSER_H_