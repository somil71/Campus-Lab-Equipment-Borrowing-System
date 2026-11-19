cmake_minimum_required(VERSION 3.15)
project(CampusLabEquipmentBorrowingSystem C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src vendor/sqlite)

add_executable(campus_lab
    src/main.cpp
    src/lending.cpp
    src/database.cpp
    vendor/sqlite/sqlite3.c
)

if (MSVC)
    target_compile_options(campus_lab PRIVATE /W4)
else()
    target_compile_options(campus_lab PRIVATE -Wall -Wextra)
    find_package(Threads REQUIRED)
    target_link_libraries(campus_lab PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
endif()
