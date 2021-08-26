#include "filter.h"

#include <iostream>
#include <sstream>

#include "../logger.h"
#include "jqutil.h"
#include "lexer.h"
#include "../settings.h"
#include "../../3rdparty/include/libjq.h"

// Public interface 
namespace jq {
	bool FilterAPI::use_full_jq = true;
	FilterAPI::FilterAPI(std::string q) : query_(q)
	{
	}

	void FilterAPI::Compile()
	{
		if(use_full_jq){
			return;
		}
		auto fchain = jq::internal::parse(query_);
		
		filter_chain_.insert(filter_chain_.end(), std::make_move_iterator(fchain.begin()),
			std::make_move_iterator(fchain.end()));

		fchain.clear();

		for (auto& f : filter_chain_) {
			//logger_info << f->ToString();
		}
	}

	std::shared_ptr<std::string> FilterAPI::Apply(std::shared_ptr<std::string> msg)
	{
		if(use_full_jq){
			if(query_.empty()){ 
				return msg;
			}
			auto result = std::make_shared<std::string>(cgoFilter(&query_[0], &(*msg)[0]));
			return result;
		}else{
		Document doc;
		doc.Parse(msg->c_str());

		if (doc.IsObject() || doc.IsArray()) {
			Value& value = doc;

			for (auto& filter : filter_chain_) {
				filter->apply(value);
			}

			auto result = internal::to_string_ptr(value);
			return result;
		}
		else {
			// Parsing failed, use plain string instead
			Value value;
			value.SetString(msg->c_str(), msg->size());

			for (auto& filter : filter_chain_) {
				//logger_info << filter->ToString();
				filter->apply(value);
			}

			auto result = internal::to_string_ptr(value);
			return result;
			}
		}
	}
}

namespace jq::internal {
	void And::apply(Value&)
	{
	}

	bool And::eval(Value& v)
	{
		for (auto op : children) {
			if (!op->eval(v)) {
				return false;
			}
		}
		return true;
	}

	string And::ToString()
	{
		string ret = "";
		for (auto& op : children) {
			ret += op->ToString() + " AND ";
		}

		// pop last And from string
		if (ret.substr(ret.size() - 4, 4) == "AND ") {
			ret = ret.substr(0, ret.size() - 4);
		}

		return ret;
	}
}

namespace jq::internal{
	ArrayIndex::ArrayIndex(int idx)
	{
		index = idx;
	}

	ArrayIndex::ArrayIndex(std::vector<std::string>& tokens, size_t low, size_t high)
	{
		if (low +2 != high) {
			logger_err << "wrong number of parameters in array index";
		}
		if (tokens[low] == "]") {
			logger_err << "throw - do not support empty [] array index operator";
		}

		if (tokens[low+2] != "]") {
			throw JQException(JQEXCEPT_NON_MATCH_BRACKETS);
		}

		if (!is_int(tokens[low+1])) {
			logger_err << "throw - array index is not an integer";
		}

		index = stoi(tokens[low+1]);
	}

	void ArrayIndex::apply(Value& v)
	{
		if (v.IsArray() && index < v.Capacity()) {
			if (!v[index].Empty()) {
				v = v[index];
				return;
			}
		}
		
		v.SetNull();
	}

	bool ArrayIndex::eval(Value&)
	{
		return false;
	}

	string ArrayIndex::ToString()
	{
		return "[" + to_string(index) + "]";
	}
	
}
namespace jq::internal {
	template<typename T>
	inline bool Compare::compare_v(string comp, T t1, T t2)
	{
		if (comp == EQUAL) {
			return t1 == t2;
		}
		else if (comp == NOT_EQ) {
			return t1 != t2;
		}
		else if (comp == SMALLER) {
			return t1 < t2;
		}
		else if (comp == SMALLER_EQ) {
			return t1 <= t2;
		}
		else if (comp == LARGER) {
			return t1 > t2;
		}
		else if (comp == LARGER_EQ) {
			return t1 >= t2;
		}
		else {
			logger_err << "no comparator specified";
		}
		return false;
	}

	Compare::Compare()
	{
	}

	Compare::Compare(vector<string>& tokens)
	{
	}

	Compare::Compare(vector<string>& tokens, size_t low, size_t high)
	{
		// At this point, we must have a clean equal operation or else will throw
		bool parsing_left_side = true;
		bool expect_keyword = false;
		for (auto i = low; i <= high; ++i) {

			if (COMPARE_OPS.find(tokens[i]) != COMPARE_OPS.end()) {
				parsing_left_side = false;
				comp_ = tokens[i];
				continue;
			}

			if (tokens[i] == "." && i != high && tokens[i + 1] == ".") {
			}
			else if (tokens[i] == "." && i != high && tokens[i + 1] == "\"") {
				// handle special index ."user.name"
			}
			else if (tokens[i] == "." && i != high) {
				// field access
				filter_ptr filter = make_shared<ObjectIdentifierIndex>(tokens[i + 1]);
				// skip the field name, just used it in previous statement
				++i;

				if (parsing_left_side) {
					left_filter.push_back(filter);
				}
				else {
					right_filter.push_back(filter);
				}
			}
			else if (tokens[i] == "[") {
				filter_ptr filter = make_shared<ArrayIndex>(tokens,i, i+2);
				
				if (parsing_left_side) {
					left_filter.push_back(filter);
				}
				else {
					right_filter.push_back(filter);
				}

				i = i + 2;
			}
			else if (tokens[i] == "\"") {

				// string
				// todo: throw if not this format " string " 

				// we have valid equal to string

				if (parsing_left_side) {
					s_.first = tokens[i + 1];
					Document document;
					left_.SetString(s_.first.c_str(), s_.first.size(), document.GetAllocator());
					left_str = s_.first;
				}
				else {
					s_.second = tokens[i + 1];
					Document document;
					right_.SetString(s_.second.c_str(), s_.second.size(), document.GetAllocator());
					right_str = s_.second;
				}

				++i; // advance to keyword
				++i; // advance to closing "
			}
			else if (is_int(tokens[i])) {
				int integer = stoi(tokens[i]);

				if (parsing_left_side) {
					left_.SetInt(integer);
				}
				else {
					right_.SetInt(integer);
				}
			}
			else if (is_double(tokens[i])) {
				double dbl = stod(tokens[i]);

				if (parsing_left_side) {
					left_.SetDouble(dbl);
				}
				else {
					right_.SetDouble(dbl);
				}
			}
			else if (tokens[i] == "true") {
				if (parsing_left_side) {
					left_.SetBool(true);
				}
				else {
					right_.SetBool(true);
				}
			}
			else if (tokens[i] == "false") {
				if (parsing_left_side) {
					left_.SetBool(false);
				}
				else {
					right_.SetBool(false);
				}
			}
			else if (tokens[i] == "null") {
				if (parsing_left_side) {
					left_.SetNull();
				}
				else {
					right_.SetNull();
				}
			}
		}

	}

	void Compare::apply(Value& v)
	{
		auto result = eval(v);

		if (result) {
			v.SetBool(true);
		}
		else {
			v.SetBool(false);
		}
	}

	bool Compare::eval(Value& v)
	{
		
		if (!left_filter.empty()) {
			Document doc;
			left_.CopyFrom(v, doc.GetAllocator());
			for (auto& f : left_filter) {
				f->apply(left_);
			}

			if (left_.IsString() && left_.GetStringLength() > 0) {
				// use this value for string comparison below
				// there is a problem and the const char * gets cleared and crashes with "memory access violation".
				left_str = left_.GetString();
			}
		}

		if (!right_filter.empty()) {
			Document doc;
			right_.SetNull();
			right_.CopyFrom(v, doc.GetAllocator());

			for (auto& f : right_filter) {
				f->apply(right_);
			}
		}

		// Evaluate for: string, int, double, boolean
		if (left_.IsString() && right_.IsString()) {
			string left("");
			string right("");

			if (left_.GetStringLength() > 0) {
				//left = left_.GetString();	// bug
				left = left_str;
			}

			if (right_.GetStringLength() > 0) {
				//right = right_.GetString(); // bug
				right = right_str;
			}
			//logger_info << left << " ? " << right;

			return compare_v<string>(comp_, std::move(left), std::move(right));
		}
		else if (left_.IsInt() && right_.IsInt()) {
			return compare_v<int>(comp_, left_.GetInt(), right_.GetInt());
		}
		else if (left_.IsDouble() && right_.IsDouble()) {
			return compare_v<double>(comp_, left_.GetDouble(), right_.GetDouble());
		}
		else if (left_.IsBool() && right_.IsBool()) {
			return compare_v<bool>(comp_, left_.GetBool(), right_.GetBool());
		}
		else if (left_.IsNull() || right_.IsNull()) {
			//comparing to null
			return compare_v<bool>(comp_, left_.IsNull(), right_.IsNull());
		}

		return false;
	}

	string Compare::ToString()
	{
		string ret;

		for (auto& f : left_filter) {
			ret += f->ToString() + " ";
		}

		if (left_.IsString()) {
			ret += left_.GetString();
		}
		else if (left_.IsInt()) {
			ret += to_string(left_.GetInt());
		}
		else if (left_.IsDouble()) {
			ret += to_string(left_.GetDouble());
		}
		else if (left_.IsBool()) {
			ret += left_.GetBool();
		}
		else if (left_.IsNull()) {
			ret += "null";
		}

		ret += comp_;

		for (auto& f : right_filter) {
			ret += f->ToString() + " ";
		}

		if (right_.IsString()) {
			ret += right_.GetString();
		}
		else if (right_.IsInt()) {
			ret += to_string(right_.GetInt());
		}
		else if (right_.IsDouble()) {
			ret += to_string(right_.GetDouble());
		}
		else if (right_.IsBool()) {
			ret += right_.GetBool();
		}
		else if (right_.IsNull()) {
			ret += "null";
		}

		return ret;
	}
}

namespace jq::internal {
	Contains::Contains(vector<string>& tokens, size_t low, size_t high)
	{
		if (tokens[low] != "contains" && tokens[low + 1] != "(") {
			logger_err << "Could not build contains(..) operation";
			print(tokens, low, high);
		}

		if (high - low == 3 &&
			tokens[low + 1] == "(" && tokens[low + 3] == ")") {
			// contains(key1)
			key = tokens[low + 2];
		}
		else if (high - low == 5 &&
			tokens[low + 1] == "(" && tokens[low + 2] == "\"" &&
			tokens[low + 4] == "\"" && tokens[low + 5] == ")") {
			// contains("key1")
			key = tokens[low + 3];
		}
		else {
			throw JQException(JQEXCEPT_MALFORMED_QUERY);
		}
	}

	void Contains::apply(Value& v)
	{
		auto str_ptr = to_string_ptr(v);
		auto found = str_ptr->find(key);

		if (found != std::string::npos) {
			v.SetBool(true);
			return;
		}
		v.SetBool(false);
	}

	bool Contains::eval(Value& v)
	{
		auto str_ptr = to_string_ptr(v);
		if (str_ptr->find(key) != std::string::npos) {
			return true;
		}
		return false;
	}

	string Contains::ToString()
	{
		return "contains(" + key + ")";
	}
}


namespace jq::internal {
	Exists::Exists(vector<string>& tokens, size_t low, size_t high)
	{
		for (auto i = low; i <= high; ++i) {

			if (tokens[i] == "." && i != high) {
				auto f = make_shared<ObjectIdentifierIndex>(tokens[i + 1]);
				filters.push_back(f);
				++i;
			}
			else if (tokens[i] == "[" && i < high - 1) {
				int index = stoi(tokens[i + 1]);
				auto f = make_shared<ArrayIndex>(index);
				filters.push_back(f);
				++i;
				++i;
			}
			else {
				//throw - should never reach this stage for this op
			}
		}
	}

	void Exists::apply(Value& v)
	{
		for (auto& f : filters) {
			f->apply(v);
		}
	}

	bool Exists::eval(Value& v)
	{
		Document doc;
		doc.CopyFrom(v, doc.GetAllocator());
		Value& result = doc;

		for (auto& f : filters) {
			f->apply(result);
		}

		if (result.IsNull()) {
			return false;
		}
		else {
			return true;
		}
	}

	string Exists::ToString()
	{
		string ret = "EXISTS ";

		for (auto& f : filters) {
			ret += f->ToString() + " ";
		}

		return ret;
	}
}

namespace jq::internal {
	void Generic::apply(Value& v)
	{
		Value ret;

		if (bool_operation) {
			auto result = bool_operation->eval(v);
			if (result) {
				
				ret.SetBool(true);
				v = std::move(ret);
			}
			else {
				ret.SetBool(false);
				v = std::move(ret);
			}
		}

		if (!children.empty()) {

			for (auto& filter : children) {
				filter->apply(v);
			}
		}
	}

	bool Generic::eval(Value& v)
	{
		Document doc;
		doc.CopyFrom(v, doc.GetAllocator());
		Value& result = doc;

		for (auto& f : children) {
			f->apply(result);
		}

		if (result.IsBool()) {
			if (result.GetBool()) {
				return true;
			}
		}

		if (result.IsString()) {
			if (result.GetString() != "") {
				return true;
			}
		}
		
		return false;
	}
	string Generic::ToString()
	{
		if (bool_operation) {
			return bool_operation->ToString();
		}

		std::stringstream ss;
		ss << "Generic: ";
		if (!children.empty()) {
			for (auto& f : children) {
				ss << f->ToString();
			}
		}
		
		return ss.str();
	}
}

namespace jq::internal {
	Not::Not(std::vector<std::string> tokens, size_t low, size_t high)
	{
		if (low != high) {
			throw JQException(JQEXCEPT_MALFORMED_QUERY);
		}
	}
	void Not::apply(Value& v)
	{
		if (v.IsBool() && v.GetBool()) {
			v.SetBool(false);
		}
		else if (v.IsObject() && v.MemberCount() != 0) {
			v.SetBool(false);
		}
		else if (!v.Empty()) {
			v.SetBool(false);
		}
		else {
			v.SetBool(true);
		}
	}

	bool Not::eval(Value& v)
	{
		Document doc;
		doc.CopyFrom(v, doc.GetAllocator());
		Value& result = doc;
		
		apply(result);

		if (result.IsBool() && result.GetBool()) {
			return true;
		}

		return false;	//unused
	}

	string Not::ToString()
	{
		return " NOT ";
	}
}

namespace jq::internal{
	ObjectIdentifierIndex::ObjectIdentifierIndex(vector<string>& tokens, size_t low, size_t high)
	{
		if (tokens.empty() || low==high || tokens[low] != ".") {
			logger_err << "cannot build field access filter";
			// throw
		}

		if (tokens[low+1] == "\"") {
			if (tokens[3] != "\"") {
				logger_err << "missing matching quotes";
				//throw
			}
			
			key = tokens[low + 2];
		}
		else {
			key = tokens[low + 1];
		}
	}

	void ObjectIdentifierIndex::apply(Value& v)
	{
		if (v.IsObject() && v.HasMember(key.data())) {
			v = v[key.data()];
		}
		else {
			v.SetNull();
		}
	}

	bool ObjectIdentifierIndex::eval(Value&)
	{
		/*
		if (v.IsObject() && v.HasMember(key.c_str())) {
			Document doc;
			doc.CopyFrom(v, doc.GetAllocator());
			Value& ret = doc;
			ret = ret[key.data()];
			return std::move(ret);
		}
		else {
			Value ret;
			return std::move(ret);
		}*/
		return false;
	}

	string ObjectIdentifierIndex::ToString()
	{
		return "." + key;
	}
}

namespace jq::internal {
	void Or::apply(Value&)
	{
	}

	bool Or::eval(Value& v)
	{
		for (auto op : children) {
			if (op->eval(v)) {
				return true;
			}
		}
		return false;
	}

	string Or::ToString()
	{
		string ret = "";
		for (auto& op : children) {
			ret += op->ToString() + " OR ";
		}

		// pop last Or from string
		if (ret.substr(ret.size() - 3, 3) == "OR ") {
			ret = ret.substr(0, ret.size() - 3);
		}

		return ret;
	}
}

namespace jq::internal {
	Regex::Regex(vector<string> tokens, size_t low, size_t high)
	{
		if (tokens[low] != "regex" && tokens[low] != "test") {
			logger_err << "could not compile regex(...)";
			print(tokens, low, high);
		}

		if (tokens[low + 1] != "(" && tokens[high] != ")") {
			logger_err << "no matching parantheses for regex(..)";
		}
		
		expr = tokens[low + 2]; // regex(expr)
		regexpr = expr;
	}

	void Regex::apply(Value& v)
	{
		auto str_ptr = to_string_ptr(v);
		std::smatch match;
		
		std::stringstream ss;

		if (std::regex_search(*str_ptr, match, regexpr)) {
			string result;
			for (size_t i = 0; i < match.size(); ++i) {
				result += match[i];
				result += " ";
			}
			if (!result.empty()) {
				result.pop_back(); // remove last empty space
			}

			Document document;
			v.SetString(result.c_str(), result.size(), document.GetAllocator());
			return;
		}

		v.SetString("");
	}

	bool Regex::eval(Value& v)
	{
		auto str_ptr = to_string_ptr(v);

		std::smatch match;
		if (std::regex_search(*str_ptr, match, regexpr)) {
			//logger_debug<< "matched regex: " << match[0] << '\n';
			return true;
		}
		return false;
	}

	string Regex::ToString()
	{
		return "regex(" + expr + ")";
	}
}

namespace jq::internal {	
	
	void Select::apply(Value& v)
	{
		if (bool_operation.size() == 0) {
		}
		else if (bool_operation.size() == 1) {
			auto result = eval(v);

			if (!result) {
				v.SetNull();
			}
		}
		else { 
			// Multiple operations means these must be piped

			Document doc;
			doc.CopyFrom(v, doc.GetAllocator());
			Value& value = doc;

			for (auto& op : bool_operation) {
				op->apply(value);
				//logger_info << op->ToString() << ", "<< to_string_ptr(value)->c_str();
			}

			if (value.IsNull()) {
				// result is null, erase v
				v.SetNull();

			}
			else if (value.IsBool() && !value.GetBool()) {
				// result is false, erase v
				v.SetNull();
			}
			else if (value.IsString() && value.GetStringLength() == 0) {
				v.SetNull();
			}
		}
	}

	bool Select::eval(Value& v)
	{
		if ((v.IsObject() && !v.ObjectEmpty()) || (v.IsArray() && !v.Empty())) {
			if (bool_operation.size() > 0 && bool_operation[0]->eval(v)) {
				return true;
			}
		}

		return false;
	}

	string Select::ToString()
	{
		std::stringstream ss;
		for (auto& b : bool_operation) {
			ss << b->ToString() << "  ";
		}
		return ss.str();
	}

}
