file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)

set(LIBX265_PATH ${3RDPART_PATH}/libx265)
include_directories(${LIBX265_PATH}/include)
link_directories(${LIBX265_PATH}/lib)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} libx265)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})
