//
// Created by Nik Babuhin on 13.09.2021.
//

#ifndef MSD_FIRE_CV_H
#define MSD_FIRE_CV_H

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


#include "detector.h"
#include "data_exchange/data_exchange.h"
#include "data_exchange/media_server.h"


#include <thread> // для работы с потоками
#include <mutex>
#include <shared_mutex> // многопользовательские мутексы
#include <future>
#include <iostream>
#include <algorithm>
#include <map>

#include <chrono>  // прекрасная библиотека работы современем завезли с последними стандартами языка c++
#include <unistd.h>


#include <opencv2/opencv.hpp>

#include <nadjieb/mjpeg_streamer.hpp>

// for convenience
using MJPEGStreamer = nadjieb::MJPEGStreamer;


using namespace cv;
using namespace std;
using namespace std::chrono;



class cv_base{
public:
    cv_base();
    Mat frame;
    bool stop_flag_0 = false;
    bool stop_flag_1 = false;
    ~cv_base();
};


struct Test_mode{
    bool Test_m = false;
    bool Frame = false;
    bool FG_Mask = false;
    bool Canny = false;
};

struct General;     /// объеквление структуры чтобы она не потерялась
//struct DNN_general; /// объявление структуры DNN
struct General_media_server; /// объявление структуры DNN


/// Общие настроки детектора движения на основе сигнатур
class settings_camera{
public:
    bool isState = true;          // Выбор режима работы CV true - USB камера false - URL
    string message = "/media/nikolai/Новый том/VF/no-photo.png"; //string URL = "/media/nikolai/Новый том/VF/port.mp4"; изображение если нет камеры
    string URL = "rtsp://admin:admin@192.168.250.217:554";
    int camIndex = 0;
};


/// Основной класс CV проверка подключения и начало работы
class cv_general:cv_base{
private:
    Test_mode testMode;
    settings_camera settings;
protected:
    VideoCapture cam;             /// пространство камеры
    vector<vector<vector<Point>>*> contours_buf;
public:
    cv_general(General *pGeneral){
        cv(pGeneral);
    }

    void cv(General *general);
    void check_of_work();             /// Найстройка параметров и проверка камеры

    ~cv_general();                    /// удаление не удачного подключения
};


void media_server_img(General_media_server *general_media_server);

#endif //MSD_FIRE_CV_H
