# With cgo, cannot use .lib directly in visual studio (only works with MinGW compiler). 
# So we need to generate the .dll file with go build, then generate the .lib file from the dll by using the function definitions 
#
# details 1: https://stackoverflow.com/questions/9946322/how-to-generate-an-import-library-lib-file-from-a-dll
# details 2: https://web.archive.org/web/20130330132630/http://www.coderetard.com/2009/01/21/generate-a-lib-from-a-dll-with-visual-studio
# 
# To create the .def file from .dll, open developer command VS2019->Tools->Command Line->Developer command prompt
# dumpbin /exports libjq.dll
# get the function names and add them to the .def file, example:
# EXPORTS
# function1
# function2
#
# After .def file is created, run command below to create .lib from .def

lib /def:libjq.def /out:libjq.lib /machine:x64
