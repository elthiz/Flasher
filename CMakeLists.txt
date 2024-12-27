cmake_minimum_required(VERSION 3.22)

project(flasher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
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

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(SRC 
    main.cpp 
    can/canopen.cpp 
    firmware/hexfirmware.cpp 
    flasher/canflasher.cpp 
    log/out.cpp 
    setting/settings.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -pthread")

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

find_library(SOCKETCAN_LIB 
    NAMES libsocketcan 
    PATHS ${LIBS_DIR} 
    REQUIRED
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(flasher PRIVATE
    can
    firmware
    flasher
    interface
    log
    setting
)

target_link_libraries(flasher PRIVATE ${SOCKETCAN_LIB})

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${LIBS_DIR}"
    BUILD_RPATH "${LIBS_DIR}"
)