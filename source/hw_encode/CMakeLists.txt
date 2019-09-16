file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})

