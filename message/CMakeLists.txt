INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB MESSAGE_SRC           *.h *.cc *.c)
FILE(GLOB PARSE_SRC           parse/*.h parse/*.cc)
FILE(GLOB GENERATE_SRC        generate/*.h generate/*.cc)
SOURCE_GROUP("message"        FILES ${MESSAGE_SRC})
SOURCE_GROUP("parse"          FILES ${PARSE_SRC})
SOURCE_GROUP("generate"       FILES ${GENERATE_SRC})

ADD_EXECUTABLE(message
  ${MESSAGE_SRC}
  ${PARSE_SRC}
  ${GENERATE_SRC}
  )
TARGET_LINK_LIBRARIES(message antlr3c)
