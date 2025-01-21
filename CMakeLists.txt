cmake_minimum_required(VERSION 3.16)
project(MSD_fire)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

##########################################################
find_library(PTHREAD_LIBRARY pthread)
##########################################################
set(OpenCV_DIR /home/pi/opencv/build)
find_package(OpenCV REQUIRED)
##########################################################
#set(CURL_ROOT_DIR /home/nikolai/curl-master/include/)
find_package(CURL REQUIRED)
##########################################################
set(nadjieb_mjpeg_streamer_DIR /home/pi/cpp-mjpeg-streamer/build)
find_package(nadjieb_mjpeg_streamer REQUIRED)
##########################################################
# Вариант с автозагрузкой пакета json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.1/json.tar.xz)
FetchContent_MakeAvailable(json)

# cv
set(cv_headers cv/cv.h cv/detector.h )
set(cv_sources cv/cv.cpp cv/detector.cpp)
# data_exchange
set(data_exchange_headers data_exchange/data_exchange.h data_exchange/base64.h data_exchange/media_server.h)
set(data_exchange_sources data_exchange/data_exchange.cpp data_exchange/base64.cpp data_exchange/media_server.cpp)
# mlx_90621
set(mlx_90621_headers mlx_90621/bmp.h mlx_90621/bmp.h )
set(mlx_90621_sources mlx_90621/mlx90621.c mlx_90621/bmp.c)

# project
set(project_headers ${cv_headers} ${data_exchange_headers}  ${mlx_90621_headers})
set(project_sources main.cpp ${cv_sources} ${data_exchange_sources} ${mlx_90621_sources})

add_executable(MSD_fire main.cpp ${project_sources} ${project_headers})

target_include_directories(MSD_fire PUBLIC ${PROJECT_INCLUDE_DIRS} ${CURL_INCLUDE_DIR})

target_link_libraries(MSD_fire
        ${TARGET_NAME}
        ${CODE_LIB_FILES}
        ${PTHREAD_LIBRARY}
        ${OpenCV_LIBS}
        ${CURL_LIBRARIES}
        ${LIBS}
        "${LIBRARIES_FROM_REFERENCES}"
        nadjieb_mjpeg_streamer::nadjieb_mjpeg_streamer
        nlohmann_json::nlohmann_json)