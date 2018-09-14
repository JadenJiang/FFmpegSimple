file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})


#MESSAGE("====,"  "${CMAKE_BINARY_DIR}," "${MODULE}," "${CMAKE_SYSTEM_NAME},")
#aux_source_directory(. aa)
#MESSAGE("===" "  ${aa}")
#MESSAGE("===" "${CMAKE_CURRENT_SOURCE_DIR},")

