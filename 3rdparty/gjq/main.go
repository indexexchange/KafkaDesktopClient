package main

import "C"
import (
	"encoding/json"
	"fmt"
	"log"
	"time"

	"github.com/itchyny/gojq"
)

//export cgoCurrentMillis
func cgoCurrentMillis() C.long {
	return C.long(time.Now().Unix())
}

//export cgoFilter
func cgoFilter(userQuery *C.char, jsonRequest *C.char) *C.char {
	q := " fromjson | " + C.GoString(userQuery)
	input := C.GoString(jsonRequest)

	query, err := gojq.Parse(q)

	if err != nil {
		return C.CString(err.Error())
	}

	iter := query.Run(input)

	// var jsonResult string
	var result []byte

	for {
		v, ok := iter.Next()
		if !ok {
			break
		}
		if err, ok := v.(error); ok {
			return C.CString(err.Error())
		}

		j, err := json.Marshal(v)

		if err == nil {
			// jsonResult += string(j)
			if len(result) == 0 {
				result = append(result, j...)
			} else {
				result = append(result, '\n')
				result = append(result, j...)
			}
		}
	}

	// cstr := C.CString(jsonResult)
	cstr := C.CString(string(result))
	return cstr
}

func main() {
	query, err := gojq.Parse(" fromjson | .glossary.GlossDiv")

	if err != nil {
		log.Fatalln(err)
	}
	// input := map[string]interface{}{"foo": []interface{}{1, 2, 3}}
	input := `{
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
                    },
					"GlossSee": "markup"
                }
            }
        }
    }
}`
	iter := query.Run(input) // or query.RunWithContext

	v, _ := iter.Next()
	b, err := json.Marshal(v)

	fmt.Println(string(b))

	for {
		v, ok := iter.Next()
		if !ok {
			break
		}
		if err, ok := v.(error); ok {
			log.Fatalln(err)
		}
		fmt.Printf("%#v\n", v)
	}
}
