cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (FFmpegSimple)

if (WIN32)
    set(SYSTEM_PLATFORM "Windows")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

elseif(UNIX AND NOT APPLE)
    set(SYSTEM_PLATFORM "Unix")

elseif(APPLE)
    set(SYSTEM_PLATFORM "Apple")

else(${CMAKE_SYSTEM_NAME} STREQUAL "XXXX")

endif(WIN32)


set(3RDPART_PATH "${CMAKE_SOURCE_DIR}/3rdparty/${SYSTEM_PLATFORM}")

set(FFMPEG_PATH "${3RDPART_PATH}/ffmpeg")
include_directories(${FFMPEG_PATH}/include)
link_directories(${FFMPEG_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} avcodec avformat avutil avdevice avfilter postproc swresample swscale)

#include_directories(${CMAKE_INCLUDE_PATH})
#link_directories(${CMAKE_LIBRARY_PATH})


add_compile_options(-std=c++11)
add_definitions(-D__STDC_CONSTANT_MACROS)  

set(PROJECTS
    capture_screen
    libx264_encode
    libx265_encode
    mediadata_parser
    print_timestamp
    audio_resample
    sdl_player
    #audio_filter
    test_cpp
    transcode
	hw_decode
	hw_encode
    remuxer
	decode_video
)


foreach(PROJECT  ${PROJECTS})
    add_subdirectory(source/${PROJECT})
endforeach(PROJECT)

#MESSAGE(">>>" ${INCLUDE_DIRECTORIES})



