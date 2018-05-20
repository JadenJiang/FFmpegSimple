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
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${FFMPEG_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${FFMPEG_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} avcodec avformat avutil avdevice avfilter postproc swresample swscale)

include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})


list(APPEND CMAKE_CXX_FLAGS "-std=c++11") 
  

set(PROJECTS
    capture_screen
    libx264_encode
    libx265_encode
    mediadata_parser
    print_timestamp
    #audio_filter
    test_cpp
)


foreach(PROJECT  ${PROJECTS})
    #MESSAGE(">>>", ${PROJECT})
    add_subdirectory(source/${PROJECT})
endforeach(PROJECT)




