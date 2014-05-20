INCLUDE_DIRECTORIES(include)

FILE(GLOB INCLUDE_SRC   include/*.h)
FILE(GLOB SRC           src/*.c)

SOURCE_GROUP("include"      FILES ${INCLUDE_SRC})
SOURCE_GROUP("src"          FILES ${SRC})


ADD_LIBRARY(antlr3c STATIC
  ${INCLUDE_SRC}
  ${SRC}
  )
