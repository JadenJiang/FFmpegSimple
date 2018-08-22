file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)


set(LIBSDL2_PATH ${3RDPART_PATH}/SDL)
include_directories(${LIBSDL2_PATH}/include)
link_directories(${LIBSDL2_PATH}/lib)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} SDL2 SDL2main)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})