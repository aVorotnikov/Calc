# CMakeList.txt: проект CMake для DllBuilder; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("DllBuilder")

set(SOURCE_LIB "main.cpp")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SinAndPow SHARED ${SOURCE_LIB})
