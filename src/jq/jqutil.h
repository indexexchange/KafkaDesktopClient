#ifndef JQUTIL_H_
#define JQUTIL_H_

#include <string>
#include <algorithm> 
#include <functional> 
#include <iostream>
#include <sstream>
#include <cctype>
#include <locale>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

namespace jq::internal {
	inline std::shared_ptr<std::string> to_string_ptr(Value& v) {

		// If Value is a string (from regex)
		if (v.IsString()) {
			std::stringstream ss;
			std::string data = v.GetString();
			if (data.empty()) {
				return std::make_shared<std::string>("");
			}

			ss << "\"" << v.GetString() << "\"";
			return std::make_shared<std::string>(ss.str());
		}

		// Value is an object
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		v.Accept(writer);
		auto result = std::make_shared<std::string>(buffer.GetString());

		return result;
	}

	inline bool empty(std::shared_ptr<std::string> s) {
		if (!s) {
			return true;
		}
		if (*s == "null" || *s == "{}" || *s == "[]" || s->empty()) {
			return true;
		}
		return false;
	}

	inline bool has_data(std::shared_ptr<std::string> s) {
		return !empty(s);
	}

	inline std::shared_ptr<std::string> to_pretty_json(std::shared_ptr<std::string> json) {
		Document d;
		d.Parse(json->c_str());
		if (d.IsObject() || d.IsArray()) {
			StringBuffer buffer;
			PrettyWriter<StringBuffer> writer(buffer);
			d.Accept(writer);
			auto result = std::make_shared<std::string>(buffer.GetString());
			return result;
		}
		
		// not json format, plain string
		return json;
	}

	inline std::shared_ptr<std::string> to_pretty_json(std::string json) {
		Document d;
		d.Parse(json.c_str());
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		d.Accept(writer);
		auto result = std::make_shared<std::string>(buffer.GetString());
		return result;
	}

	inline bool is_int(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
	inline bool is_double(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && (std::isdigit(*it) || *it == '.')) ++it;
		return !s.empty() && it == s.end();
	}

	// trim from end
	inline std::string& rtrim(std::string& s) {
		//s.erase(s.begin(), std::find_if(s.rbegin(), s.rend(), [](auto c) {return !std::isspace(c);}));
		auto it = std::find_if(s.rbegin(), s.rend(), [](auto c) {return !std::isspace(c);});
		s.erase(it.base(), s.end());
		return s;
	}
	// trim from start
	inline std::string& ltrim(std::string& s) {
		//s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
		auto it = std::find_if(s.begin(), s.end(), [](auto c) {return !std::isspace(c);});
		s.erase(s.begin(), it);
		return s;
	}

	inline void print(std::vector<std::string> vec) {
		for (auto& v : vec) {
			std::cout << v << " ";
		}std::cout << std::endl;
	}
	inline void print(std::vector<std::string> vec, size_t low, size_t high) {
		for (auto i = low; i <= high; ++i) {
			std::cout << vec[i] << " ";
		}std::cout << std::endl;
	}
	inline bool isIn(std::vector<std::string>& vec, std::string str) {
		for (auto& v : vec) {
			if (v == str) {
				return true;
			}
		}
		return false;
	}
	inline size_t count_occurence(std::vector<std::string> vec, size_t low, size_t high, std::string str) {
		size_t res = 0;
		for (auto i = low; i <= high; ++i) {
			if (vec[i] == str) {
				++res;
			}
		}
		return res;
	}
	inline size_t find_next_match(std::vector<std::string> vec, size_t low, size_t high, std::string str) {
		for (auto i = low; i <= high; ++i) {
			if (vec[i] == str) {
				return i;
			}
		}
		return low;
	}
	inline size_t find_corresponding_match(std::vector<std::string> vec, size_t low, size_t high, std::string token1, std::string token2) {
		auto count = 0;
		for (auto i = low; i <= high; ++i) {
			if (vec[i] == token1) {
				++count;
			}
			if (vec[i] == token2) {
				if (count == 1) {
					return i;
				}
				else {
					--count;
				}
			}
		}
		return low;
	}
}



#endif // JQUTIL_H_