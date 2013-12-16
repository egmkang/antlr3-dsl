CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(message)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libantlr3c-3.4/include)


SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

SET(CXX_FLAGS
    -g
    -Wall
    -Werror
    -fomit-frame-pointer
    -fstack-protector-all
    -Wno-unused-parameter
    -fno-rtti
    -fno-exceptions
    )
STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
MESSAGE(STATUS "CXX flags : " "${CMAKE_CXX_FLAGS}")

SET(CMAKE_CXX_FLAGS_DEBUG   "-DDEBUG -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2")

ADD_SUBDIRECTORY(libantlr3c-3.4)

ADD_SUBDIRECTORY(expr)
ADD_SUBDIRECTORY(message)
