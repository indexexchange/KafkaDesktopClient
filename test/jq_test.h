#ifndef JQ_TEST_H
#define JQ_TEST_H

#include <string>

std::string run_query(std::string);
std::string run_query(std::string, std::string);
std::string run_query_on_array(std::string);


std::string get_exception(std::string);
std::string get_exception(std::string, std::string);
std::string get_exception_on_array(std::string);

const std::string kafka_json = R"###({
    "k1":"v1",
    "k2":"v2",
    "k3":"v3",
    "pi": 3.14,
    "bool_true": true,
    "bool_false": false,
    "adunits": null,
    "browser": "SAF",
    "country": "CA",
    "errno": null,
    "inventory_channel": "book",
    "os": "IOS",
    "id": 184070,
    "raw": null,
    "stages": [
        {
            "stage": 9
        }
    ],
    "[key1.key2]" : {
        "[key3.key4]" :  ["composite_keys_result"]
    },
    "object": {
        "[some.request]": {
            "QueryId": "APB"
        },
        "ext": {
            "source": "extsource"
        },
        "id": 25526174,
        "array": [
            {
                "rectangle": {
                    "h": 50,
                    "topframe": 1,
                    "w": 300
                },
                "ext": {
                    "sid": "25",
                    "sid2": "222080"
                },
                "id": "1"
            },
            {
                "rectangle": {
                    "h": 50,
                    "topframe": 1,
                    "w": 320
                },
                "ext": {
                    "sid": "26",
                    "sid2": "222081"
                },
                "id": "2"
            },
            {
                "rectangle": {
                    "h": 100,
                    "topframe": 1,
                    "w": 320
                },
                "ext": {
                    "sid": "29",
                    "sid2": "373723"
                },
                "id": "3"
            }
        ],
        "book": {
            "page": "https://www.msn.com/"
        },
        "user": {
            "eids": [
                {
                    "source": "google.com",
                    "uids": [
                        {
                            "ext": {
                                "key": "TDID"
                            }                        
                        }
                    ]
                }
            ]
        }
    },
    "request_headers": {
        "X-Forwarded-For": [
            "1.2.3.4, 5.6.7.8"
        ],
        "X-Forwarded-Host": [
            "google.com"
        ]
    },
    "my_request_id": null,
    "response": {
        "id": "25526174"
    },
    "response_headers": {
    },
    "response_length": 18,
    "source": "w",
    "timestamp": 1593527414,
    "trace": "brtkotn7fltsn85j361g",
    "handy":"man",
    "warnings": null
})###";


const std::string kafka_json_array = R"###(
[
    "",
    "5fd3a43a",
    "460422",
    "3385959",
    "12345",
    "12345",
    "RI",
    "3.56",
    "MAC",
    "EN",
    "CHR",
    "1a",
    "0",
    "",
    "abcd",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "103",
    "",
    "3.56",
    "",
    "2052",
    "3677093907",
    "0",
    "2.67000",
    "",
    "412f8",
    "P2ee",
    "",
    "",
    "",
    "9",
    "",
    "103",
    "00000003823E457E72BB2D00000",
    "",
    "google.com",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "02",
    "",
    "",
    "",
    "",
    "",
    "",
    "644d7d8d-46c9-4518-8ee9-d6398d6b6fb2~63~8",
    "",
    "",
    "",
    "",
    "",
    "",
    "524288",
    "3137271",
    "3137271_1",
    "",
    "",
    "",
    "",
    "afc",
    "",
    "",
    "",
    "",
    "",
    "",
    "0",
    "5fd3a439",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "US",
    "CHR",
    "MAC",
    "",
    "02",
    "55",
    "0",
    "",
    "01571271411491511831551bd19b1331c316316a15a19c1851401b21a31,039b3c305e3b23a338338534939c3413693573273",
    "",
    "3",
    "",
    "p",
    "",
    "IL",
    "60126",
    "602",
    "XX",
    "Mozilla",
    "01",
    "",
    "buy.com",
    "00",
    "0.00",
    "4250af1c-4a2f-a40f-681c-d445f4a1055c",
    "",
    "",
    "6",
    "0",
    "0.00",
    "",
    "",
    "",
    "02",
    "",
    "",
    "",
    "",
    "10",
    "",
    "",
    "191306",
    "183208",
    "0",
    "OS X",
    "Chrome",
    "87"
]
)###";



#endif // JQ_TEST_H