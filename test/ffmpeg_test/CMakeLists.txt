cmake_minimum_required(VERSION 3.21)

project(myDecode)
set(TARGET myDecode)
set(CMAKE_CXX_STANDARD 17)
set(ROOT_PATH ${CMAKE_SOURCE_DIR})

add_definitions(-DRESOURCE_DIR="${ROOT_PATH}/../../resource")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(FFMPEG_INCLUDE_PATH "${ROOT_PATH}/../../thirdparty/ffmpeg/mac/include")
    set(FFMPEG_LIB_PATH "${ROOT_PATH}/../../thirdparty/ffmpeg/mac/lib")


    set(FFMPEG_LINK_LIBS ${FFMPEG_LIB_PATH}/libavformat.a
        ${FFMPEG_LIB_PATH}/libavdevice.a
        ${FFMPEG_LIB_PATH}/libavcodec.a
        ${FFMPEG_LIB_PATH}/libavfilter.a
        ${FFMPEG_LIB_PATH}/libavutil.a
        ${FFMPEG_LIB_PATH}/libswresample.a
        ${FFMPEG_LIB_PATH}/libswscale.a)

    set(FFMPEG_LINK_LIBS ${FFMPEG_LINK_LIBS}
        libz.a
        libbz2.a
        libiconv.a
        lzma
        "-framework AudioToolbox"
        "-framework VideoToolbox"
        "-framework CoreVideo"
        "-framework CoreAudio"
        "-framework CoreMedia"
        "-framework CoreImage"
        "-framework CoreFoundation"
        "-framework AppKit"
        "-framework OpenGL"
        "-framework Security")

endif()

set(BASE_HDR
    ""
)

set(BASE_SRC
    main.cpp
    avccReader.cpp
    avcDecoder.cpp
)

add_executable(${TARGET} ${BASE_HDR} ${BASE_SRC})
target_include_directories(${TARGET} PUBLIC ${HDR_DIR} ${FFMPEG_INCLUDE_PATH})
target_link_libraries(${TARGET} ${FFMPEG_LINK_LIBS})

message("------ ijk base project end() ------ \n")
