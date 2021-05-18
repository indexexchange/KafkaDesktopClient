#include <string>
#include <memory>

#include "gtest/gtest.h"

#include "../src/jq/filter.h"
#include "jq_test.h"

/*
TODO:
    - support for comparing negative numbers
    - support conversion from int to double: .pi > 2 vs pi > 2.0
    - accepting both string and number if the input is a number (ie. .key == 2 vs .key == "2")
*/


// AND

TEST(jqSuite, AND) {

    EXPECT_EQ("true", run_query(R"#(  .k1 == "v1" and .k2 == "v2"  )#"));
    EXPECT_EQ("true", run_query(R"#(  .k1 == "v1" && .k2 == "v2"  )#"));
    EXPECT_EQ("true", run_query(R"#(  .k1 == "v1" AND .k2 == "v2" AND .k3 == "v3"  )#"));
    EXPECT_EQ("false", run_query(R"#(  .k1 == "v1" and .k2 == "v3"  )#"));

    EXPECT_EQ("true", run_query(R"#(  (.k1 == "v1" AND  .k2 == "v2") AND .k3 == "v3"  )#"));
    EXPECT_EQ("true", run_query(R"#( (.k1 == "v1" AND  .k2 == "v2") OR (.k3 != "v3")  )#"));
    EXPECT_EQ("true", run_query(R"#( (.k1 != "v1" AND  .k2 != "v2") OR (.k3 == "v3")  )#"));
    EXPECT_EQ("false", run_query(R"#( (.k1 != "v1" AND  .k2 != "v2") OR (.k3 != "v3")  )#"));

    EXPECT_EQ(JQEXCEPT_MALFORMED_QUERY, get_exception(R"#( .k2 == "v2" and k1 == "v1"  )#"));
}

// ARRAY ACCESS
TEST(jqSuite, ArrayIndexAccess) {

    EXPECT_EQ("null", run_query(".object.array.[5]"));
    EXPECT_EQ("null", run_query(".object.array[5]"));
    EXPECT_EQ("null", run_query(".object.array[3]"));
    EXPECT_EQ("50", run_query(".object.array[0].rectangle.h"));
    EXPECT_EQ("50", run_query(".object.array.[0].rectangle.h"));
    EXPECT_EQ("50", run_query(".object.array|[0]|.rectangle.h"));

    EXPECT_EQ("\"RI\"", run_query_on_array("[6]"));
    EXPECT_EQ("\"RI\"", run_query_on_array(".[6]"));

    EXPECT_EQ("true", run_query_on_array("[6] == \"RI\""));
    EXPECT_EQ("true", run_query_on_array(".[6] == \"RI\""));

    EXPECT_EQ("\"RI\"", run_query_on_array("select([6] == \"RI\") | [6]"));
    EXPECT_EQ("true", run_query_on_array("[110] == \"4250af1c-4a2f-a40f-681c-d445f4a1055c\""));
    EXPECT_EQ("\"5fd3a43a\"", run_query_on_array("select([110] == \"4250af1c-4a2f-a40f-681c-d445f4a1055c\") | [1]"));

    EXPECT_EQ(JQEXCEPT_ARRAY_INDX_NOT_INT, get_exception(R"#(  .key[[]]  )#"));
    EXPECT_EQ(JQEXCEPT_ARRAY_INDX_NOT_INT, get_exception(R"#(  .key[a]  )#"));
}

// COMPARISON
TEST(jqSuite, BooleanOps) {

    // Integers
    EXPECT_EQ("true", run_query(".object.id == 25526174"));
    EXPECT_EQ("true", run_query(".object.id < 25526175"));
    EXPECT_EQ("true", run_query(".object.id <= 25526175"));
    EXPECT_EQ("false",run_query(".object.id == 0"));
    EXPECT_EQ("true", run_query(".object.id > 1000"));
    EXPECT_EQ("true", run_query(".object.id >= 1000"));
    EXPECT_EQ("true", run_query(".object.id != 100"));

    // Double
    //- test suite for all boolean operators (strings)
    EXPECT_EQ("true", run_query(".pi == 3.14"));
    EXPECT_EQ("false",run_query(".pi != 3.14"));
    EXPECT_EQ("true", run_query(".pi > 2.0"));
    EXPECT_EQ("true", run_query(".pi >= 2.0"));
    EXPECT_EQ("true", run_query(".pi < 4.0"));
    EXPECT_EQ("true", run_query(".pi <= 4.0"));
    //EXPECT_EQ("true", run_query(".pi > 2"));      // FIX - convert from int to double

    // Boolean values
    EXPECT_EQ("true", run_query(".bool_true == true"));
    EXPECT_EQ("false",run_query(".bool_true != true"));
    EXPECT_EQ("true", run_query(".bool_false == false"));

    // Strings
    EXPECT_EQ("false", run_query(".k1 != \"v1\""));

    // Other
    EXPECT_EQ("true", run_query(".stages[0].stage == 9"));
    EXPECT_EQ("false",run_query(".stages[0].stage != 9"));
    EXPECT_EQ("true", run_query(".stages[0].stage >= 9"));
    EXPECT_EQ("true", run_query(".stages[0].stage < 10"));
    EXPECT_EQ("true", run_query(".stages[0].stage > 0"));
}

// CONTAINS 
TEST(jqSuite, Contains) {

    EXPECT_EQ("false", run_query("contains(_does_not_exist_)"));
    EXPECT_EQ("false", run_query("CONTAINS(_does_not_exist_)"));
    EXPECT_EQ("true", run_query("contains(my_request_id)"));
    EXPECT_EQ("true", run_query("CONTAINS(my_request_id)"));
    EXPECT_EQ("null", run_query("select(contains(_does_not_exist_)) | .id"));
    EXPECT_EQ("184070", run_query("select(contains(request_id)) | .id"));
    EXPECT_EQ("184070", run_query("select((.object.book.page | contains(msn)) and (.id == 184070)) | .id"));

    // Contains used with pipe
    EXPECT_EQ("184070", run_query(R"#( select(.object.array[0].rectangle | contains(top)) | .id  )#"));
    EXPECT_EQ("null", run_query(R"#( select(.object.array[0].rectangle | contains(_non_existent_)) | .id )#"));

}

// FIELD ACCESS
TEST(jqSuite, FieldAccess) {
    
    EXPECT_EQ("\"man\"", run_query(".handy"));
    EXPECT_EQ("null",   run_query(".horba"));
    EXPECT_EQ("\"CA\"", run_query(".country"));
    EXPECT_EQ("25526174", run_query(".object.id"));
    EXPECT_EQ("50",   run_query(".object.array[0].rectangle.h"));
    EXPECT_EQ("null", run_query(".object.array[0].rectangle.hhh"));
    EXPECT_EQ("\"extsource\"", run_query(".object.ext.source"));
    EXPECT_EQ("null", run_query(".object.ext.source_noexist"));

    // Special keys marked with quotes
    EXPECT_EQ("\"APB\"", run_query(R"#( .object."[some.request]".QueryId )#"));
    EXPECT_EQ("\"composite_keys_result\"", run_query(R"#( ."[key1.key2]"."[key3.key4]"[0])#"));
}

// NOT
TEST(jqSuite, NOT){
    EXPECT_EQ("true", run_query(".handy | not | not "));
    EXPECT_EQ("184070", run_query(R"#(  select(.k1 == "no_exist" | not) | .id     )#"  ));
    EXPECT_EQ("null",   run_query(R"#(  select(.k1 == "v1" | not) | .id  )#"));
    EXPECT_EQ("false",  run_query(".object.array[0].rectangle.h | not"));
    EXPECT_EQ("true",   run_query(".object.array[0].rectangle.h == 50"));
    EXPECT_EQ("false",  run_query(".object.array[0].rectangle.h == 50 | not"));
    EXPECT_EQ("null",   run_query("select(.handy | not) | .id"));
    EXPECT_EQ("184070", run_query("select(.not_exist | not) | .id"));
}
// OR
TEST(jqSuite, OR) {

    EXPECT_EQ("false", run_query(R"#( (.k1 != "v1" OR .k2 == "v2") AND (.k3 != "v3")  )#"));
    EXPECT_EQ("true",  run_query(R"#( (.k1 != "v1" OR .k2 == "v2") AND (.k3 == "v3")  )#"));
    EXPECT_EQ("false", run_query(R"#( (.k1=="_" OR .k2=="_") AND (.k1=="_" OR .k2=="_")  )#"));
    EXPECT_EQ("true",  run_query(R"#( (.k1=="_" OR .k2=="v2") AND (.k1=="v1" OR .k2=="_")  )#"));
}

// PIPE SUITE 
TEST(jqSuite, Piping) {

    EXPECT_EQ("\"extsource\"", run_query(".object | .ext.source"));
    EXPECT_EQ("50",     run_query(".object | .array[0] | .rectangle.h"));
    EXPECT_EQ("184070", run_query("select(.source==\"w\")|select(.object.array[0]) | .id"));
    EXPECT_EQ("50",     run_query(".object | select (.id == 25526174) | select(.array[1]) | .array[0].rectangle.h"));
}

// REGEX SUITE 
TEST(jqSuite, Regex) {

    EXPECT_EQ("\"os\":\"IOS\"", run_query("regex(os.*?IOS)"));
    EXPECT_EQ("\"os\":\"IOS\"", run_query("REGEX(os.*?IOS)"));
    EXPECT_EQ("\"os\":\"IOS\"", run_query("test(os.*?IOS)"));
    EXPECT_EQ("\"os\":\"IOS\"", run_query("TEST(os.*?IOS)"));

    EXPECT_EQ("", run_query("regex(os.*?AND)"));
    EXPECT_EQ("184070", run_query("select(regex(os.*?IOS)) | .id"));
    EXPECT_EQ("null",   run_query("select(regex(os.*?AND)) | .id"));
    EXPECT_EQ("null",   run_query("select(REGEX(os.*?AND)) | .id"));

    // Regex used with pipe
    EXPECT_EQ("null", run_query(R"#( select(.object.array[0].rectangle | regex(does_not_exist)) )#"));
    EXPECT_EQ("184070", run_query(R"#( select(.object.array[0].rectangle | regex(top.*) | regex(top) | contains(top) ) | .id )#"));
    EXPECT_EQ("184070", run_query("select(.request_headers | regex(X-Forwarded-Host)) | .id"));
}

// SELECTION SUITE
TEST(jqSuite, Selection) {

    EXPECT_EQ("184070", run_query("select(.country == \"CA\") | .id"));
    EXPECT_EQ("null",   run_query(R"#(  select(.k1 == "v1" | not) | .id     )#"));
    EXPECT_EQ("null",       run_query("select(.country == \"US\") | .id"));
    EXPECT_EQ("184070", run_query("select(.object.array[0]) | .id"));
    EXPECT_EQ("null",   run_query("select((.object.book.page | contains(bbbb)) and (.id == 184070)) | .id"));
    EXPECT_EQ("184070", run_query("select((.object.book.page | contains(msn)) and (.id == 184070)) | .id"));
    EXPECT_EQ("184070", run_query(R"#(  select(.browser == "SAF" and .object.user | contains(TDID)) | .id )#"));
    EXPECT_EQ("null",   run_query(R"#(  select(.browser == "IE" and .object.user | contains(TDID)) | .id )#"));

    EXPECT_EQ("null",   run_query(R"#( select(.handy == null) | .id )#"));
    EXPECT_EQ("184070", run_query(R"#( select(.object.array[20] == null) | .id )#"));
    EXPECT_EQ("184070", run_query(R"#( select(.handy != null) | .id )#"));
}

TEST(jqSuite, ExceptNoMatchQuotes) {

    EXPECT_EQ(JQEXCEPT_NON_MATCH_QUOTES, get_exception(R"(  ."special-key  )"));
    EXPECT_EQ(JQEXCEPT_NON_MATCH_QUOTES, get_exception(R"(  ."special-key"" )"));
}

TEST(jqSuite, ExceptNoMatchParanth) {

    EXPECT_EQ(JQEXCEPT_NON_MATCH_PARANTH, get_exception(R"#(select(.key1)#"));
    EXPECT_EQ(JQEXCEPT_NON_MATCH_PARANTH, get_exception(R"#( .key1) )#"));
    EXPECT_EQ(JQEXCEPT_NON_MATCH_PARANTH, get_exception(R"#( (select(.key1)))) )#"));
}

TEST(jqSuite, ExceptNoMatchBrackets) {

    EXPECT_EQ(JQEXCEPT_NON_MATCH_BRACKETS, get_exception(R"#(  .key[0  )#"));
    EXPECT_EQ(JQEXCEPT_NON_MATCH_BRACKETS, get_exception(R"#(  .key[0[0]  )#"));
}


// BAD QUERIES 
TEST(jqSuite, ExceptMalformedQuery) {

    EXPECT_EQ(JQEXCEPT_MALFORMED_QUERY, get_exception(R"#(  .key[0[0]]  )#"));    
    EXPECT_EQ(JQEXCEPT_MALFORMED_QUERY, get_exception("request"));
    EXPECT_EQ(JQEXCEPT_MALFORMED_QUERY, get_exception(".object.array|[0]|rectangle.h"));
}

TEST(jqSuite, Dev) { 

}

// Helper function to get string after filter is applied
std::string run_query(std::string query) {
    return run_query(query,kafka_json);
}

std::string run_query_on_array(std::string query) {
    return run_query(query, kafka_json_array);
}

// Helper function to get string after filter is applied
std::string run_query(std::string query, std::string request) {

    jq::FilterAPI filter(query);

    filter.Compile();

    auto json = std::make_shared<std::string>(request);
    auto result = filter.Apply(json);

    if (result) {
        return *result;
    }

    return "";
}

// Helper function to get exception when one is expected
std::string get_exception(std::string query) {
    return get_exception(query, kafka_json);
}
std::string get_exception_on_array(std::string query) {
    return get_exception(query, kafka_json_array);
}
std::string get_exception(std::string query, std::string request) {
    std::string message;
    jq::FilterAPI filter(query);

    try {
        filter.Compile();
    }
    catch (JQException& e) {
        message = e.what();
    }
    return message;
}

/*
// Benchmark a function
auto start = std::chrono::high_resolution_clock::now();
function();
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
logger_info << ": " << duration;
*/