cmake_minimum_required(VERSION 3.10)
project(llp2 C)

set(CMAKE_C_STANDARD 99)

add_executable(llp2 main.c database/database.h database/table.h database/table.c database/file_api.h database/file_api.c database/data.c database/data.h database/iterator.c database/iterator.h parser/parser.c parser/parser.h)
