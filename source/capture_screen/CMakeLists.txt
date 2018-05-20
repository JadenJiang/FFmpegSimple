file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")

set(SUFFIX  *.cpp *.h)

set(SDL_PATH ${3RDPART_PATH}/SDL)
include_directories(${SDL_PATH}/include)
link_directories(${SDL_PATH}/lib)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} SDL SDLmain)

file(GLOB SRCS ${SUFFIX})
add_executable(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})

