//
// Created by nikolai on 03.09.2021.
//

#ifndef VF_TRACKING_MEDIA_SERVER_H
#define VF_TRACKING_MEDIA_SERVER_H

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
//#include <opencv2/xfeatures2d.hpp>
#include <opencv2/video/background_segm.hpp>
//#include <opencv2/bgsegm.hpp>

#include <thread> // для работы с потоками
#include <mutex>
#include <shared_mutex> // многопользовательские мутексы
#include <future>
#include <iostream>
#include <algorithm>
#include <map>

#include <chrono>  // прекрасная библиотека работы современем завезли с последними стандартами языка c++
#include <unistd.h>

#include <nadjieb/mjpeg_streamer.hpp>


using namespace std;
using MJPEGStreamer = nadjieb::MJPEGStreamer;
using namespace cv;


struct General_media_server
{
    bool status_data = false;
    shared_mutex media_server_mutex;
    std::vector<uchar> buff_bgr;
};


class media_server
{
public:
    media_server(General_media_server *media_server_general){
        server(media_server_general);
    }
    void server(General_media_server *media_server_general);

    ~media_server();
};




#endif //VF_TRACKING_MEDIA_SERVER_H
