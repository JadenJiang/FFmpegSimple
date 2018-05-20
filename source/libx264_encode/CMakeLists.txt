file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)

set(LIBX264_PATH ${3RDPART_PATH}/libx264)
include_directories(${LIBX264_PATH}/include)
link_directories(${LIBX264_PATH}/lib)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} libx264)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})

