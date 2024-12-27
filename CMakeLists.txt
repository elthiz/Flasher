cmake_minimum_required(VERSION 3.22)

project(Flasher 
    VERSION 1.1
    DESCRIPTION "Flasher modules K15" 
    LANGUAGES CXX C)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -pthread")

option(COMPILE_WARNINGS "Enable strict compile warnings" ON)

if (COMPILE_WARNINGS)
    message(STATUS "Предупрждения компилятора включны.")
    SET(WARNING_COMPILE_FLAGS
    -Werror
    -Wall
    -Wextra
    -Wpedantic
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
    )
else()
    SET(WARNING_COMPILE_FLAGS)
    message(STATUS "Предупрждения компилятора отключены.")
endif()

add_library(libsocketcan STATIC libs/libsocketcan/src/libsocketcan.c libs/libsocketcan/header/libsocketcan.h)
target_include_directories(libsocketcan PRIVATE libs)

set(SRC 
    app/main.cpp
    app/src/can/canflasher.cpp 
    app/src/can/canopen.cpp 
    app/src/log/out.cpp 
    app/src/parser/hexparserfirmware.cpp 
    app/src/setting/settings.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_COMPILE_FLAGS})

target_include_directories(${PROJECT_NAME} PRIVATE
    app/include/can
    app/include/log
    app/include/parser
    app/include/setting
    app/include/logic
)

target_link_libraries(${PROJECT_NAME} PRIVATE libsocketcan)
