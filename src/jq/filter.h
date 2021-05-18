#ifndef FILTER_H_
#define FILTER_H_

#include <vector>
#include <memory>
#include <regex>
#include <string>

#include "rapidjson/document.h"

#include "common.h"
#include "jqutil.h"

#define string_ptr std::shared_ptr<std::string>

using namespace std;
using namespace rapidjson;

namespace jq::internal {
	class Filter {
	public:
		std::vector<std::shared_ptr<Filter>> children;
		virtual void apply(Value&) = 0;
		virtual bool eval(Value&) = 0;
		virtual string ToString() = 0;
	};

	using filter_ptr = shared_ptr<Filter>;

	class And : public Filter {
	public:
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class ArrayIndex : public Filter {
		size_t index;
		Value empty;
	public:
		ArrayIndex(int);
		ArrayIndex(std::vector<std::string>&, size_t, size_t);
		virtual void apply(Value& v);
		virtual bool eval(Value&);
		string ToString();
	};

	class Compare : public Filter {
		vector<filter_ptr> left_filter;
		vector<filter_ptr> right_filter;
		Value left_;
		Value right_;
		string left_str;	// workaround for an issue inside rapidJSON lib
		string right_str;	// workaround for an issue inside rapidJSON lib

		// comparison operator
		string comp_;

		// Holders for data
		pair<string, string> s_;
		pair<int, int> i_;
		pair<double, double> d_;
		pair<bool, bool> B_;

		template<typename T>
		bool compare_v(string, T, T);

	public:

		Compare();
		Compare(vector<string>& tokens);
		Compare(vector<string>& tokens, size_t low, size_t high);

		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class Contains : public Filter {
		string key;
	public:
		Contains(vector<string>& tokens, size_t low, size_t high);
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class Exists : public Filter {
		vector<filter_ptr> filters;
	public:
		Exists(vector<string>& tokens, size_t low, size_t high);
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class Generic : public Filter {
	public:
		filter_ptr bool_operation;
		Generic() {}
		virtual void apply(Value& v);
		virtual bool eval(Value&);
		string ToString();
	};

	class Not : public Filter {
	public:
		Not(std::vector<std::string>,size_t,size_t);
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class ObjectIdentifierIndex : public Filter {
		string key;
		Value empty;
	public:
		ObjectIdentifierIndex(string k) : key(k) {}
		ObjectIdentifierIndex(vector<string>&,size_t,size_t);
		virtual void apply(Value& v);
		virtual bool eval(Value&);
		string ToString();
	};

	class Or : public Filter {
	public:
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class Regex : public Filter {
		std::regex regexpr;
		string expr;
	public:
		Regex(vector<string> tokens, size_t low, size_t high);
		virtual void apply(Value&);
		virtual bool eval(Value& v);
		virtual string ToString();
	};

	class Select : public Filter {
		Value empty;
	public:
		std::vector<filter_ptr> bool_operation;
		Select() {}
		virtual void apply(Value& v);
		virtual bool eval(Value&);
		string ToString();
	};
}

namespace jq {

	class FilterAPI {
		std::string query_;
		std::vector<jq::internal::filter_ptr> filter_chain_;
	public:
		FilterAPI(std::string);
		void Compile();
		std::shared_ptr<std::string> Apply(std::shared_ptr<std::string>);
	};
}

namespace jq {
	inline std::shared_ptr<jq::FilterAPI> FilterPtr(std::string query) { return std::make_shared<jq::FilterAPI>(query); }
	inline string_ptr pretty_json(string_ptr json) { return internal::to_pretty_json(json); }
	inline string_ptr pretty_json(std::string json) { return internal::to_pretty_json(json); }
	inline bool empty(string_ptr json) { return internal::empty(json); }
	inline bool has_data(string_ptr json) { return internal::has_data(json); }
}

#endif // FILTER_H_