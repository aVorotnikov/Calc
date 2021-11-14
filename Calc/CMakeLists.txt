# CMakeList.txt: проект CMake для Calc; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project ("Calc")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Calc "main.cpp" "calc.h" "calc.cpp" "operation.h" "token.h"  )

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
