cmake_minimum_required(VERSION 3.16)
project(MSD_fire)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

##########################################################
find_library(PTHREAD_LIBRARY pthread)
##########################################################
set(OpenCV_DIR /home/pi/opencv/build)
find_package(OpenCV REQUIRED)
if(OpenCV_VERSION VERSION_GREATER 4.5.3)
    target_compile_definitions(library PUBLIC USE_LEGACY_TRACKING=1)
endif()
if(NOT OpenCV_FOUND)
    message(SEND_ERROR "Failed to find OpenCV")
    return()
else()
    include_directories(${OpenCV_INCLUDE_DIR})
    ocv_include_directories("${OpenCV_SOURCE_DIR}/")
    ocv_include_modules_recurse(${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})
endif()
##########################################################
#set(CURL_ROOT_DIR /home/nikolai/curl-master/include/)
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    #set(CURL_ROOT_DIR /home/nikolai/curl-7.72.0/include/)
    #set(CURL_ROOT_DIR /home/finder/Desktop/curl-7.72.0/include/)
    #set(CURL_ROOT_DIR /home/nikolai/Desktop/curl-7.78.0/include/)
    set(CURL_ROOT_DIR /home/nikolai/curl-master/include/)
    message(SEND_ERROR "Failed to find CURL")
    return()
else()
    #include_directories(${CURL_INCLUDE_DIR})
    #ocv_include_directories("${CURL_SOURCE_DIR}/")
    #ocv_include_modules_recurse(${OPENCV_CPP_SAMPLES_REQUIRED_DEPS})   прописать пути
endif()
##########################################################

# cv
set(cv_headers cv/cv.h cv/detector.h )
set(cv_sources cv/cv.cpp cv/detector.cpp)
# data_exchange
set(data_exchange_headers data_exchange/data_exchange.h data_exchange/base64.h)
set(data_exchange_sources data_exchange/data_exchange.cpp data_exchange/base64.cpp)

# project
set(project_headers ${cv_headers} ${data_exchange_headers})
set(project_sources main.cpp ${cv_sources} ${data_exchange_sources})

add_executable(MSD_fire main.cpp ${project_sources} ${project_headers})

target_include_directories(MSD_fire PUBLIC ${PROJECT_INCLUDE_DIRS} ${CURL_INCLUDE_DIR})

target_link_libraries(MSD_fire
        ${TARGET_NAME}
        ${CODE_LIB_FILES}
        ${PTHREAD_LIBRARY}
        ${OpenCV_LIBS}
        ${CURL_LIBRARIES}
        ${LIBS}
        "${LIBRARIES_FROM_REFERENCES}")