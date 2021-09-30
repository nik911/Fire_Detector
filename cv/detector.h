//
// Created by Nik Babuhin on 13.09.2021.
//

#ifndef MSD_FIRE_DETECTOR_H
#define MSD_FIRE_DETECTOR_H

extern "C" void IRService();
extern "C" void IRService_vd();

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
//#include <opencv2/xfeatures2d.hpp>

#include <iostream>
#include <thread> // для работы с потоками
#include <mutex>
#include <shared_mutex> // многопользовательские мутексы
#include <future>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
//#include <opencv2/bgsegm.hpp> и

#include "data_exchange/data_exchange.h"
#include "mlx_90621/mlx90621.h"

using namespace cv;
using namespace std;


class fire_object{
public:
    unsigned int counter=0;         /// существование объекта
    unsigned int counter_time=0;    /// на удаление
    bool traking = false;
    Mat frame_object;
    Rect rect_FireData;
    vector<double> area;
    vector<double> centerX;
    vector<double> centerY;
};


/// Общие настроки детектора движения на основе сигнатур
class settings_detector_sign
{
public:
    int type_subtractor = 0;
    double min_area = 1500;                 /// установкака порога площади под контуром
    double max_area = 10000000;               /// установкака порога площади под контуром
    bool Test_mode = true;
    int time_ms = 0;                        /// sleep
};


/// класс детектирования подвижных объектов
class cv_detection_moving {
private:
    //ROI Area
    double Area1=0, Area2=0, Area3=0;
    Rect three;
    double centerX = 0, centerY = 0;
    data_exchange exchange;
    Mat frame_out, boat_img;
    /// параметры и настройка
    bool State_img = true;
    Mat frame_in, fgMask, fgMask_ar;        /// пространство изображений ----
    settings_detector_sign  settings_sign;  /// параметры системы детектирования
    Ptr<BackgroundSubtractor> pBackSub;     /// пространство для детектируемых сигнатур
public:
    vector<fire_object> tracking_fire;         /// вектор объектов c огнем
    /// параметры для синхронизации
    vector<vector<Point>> contours_prev, contours_curr; /// вектора контуров найденые и отсеиные по полезности
    int count_contour = 0;                              /// счётчик текущих контуров
    int count_largest_contour = 0;
    vector<int> contours_detects;                       /// индексы соответсвующих контов

    cv_detection_moving();                       /// настрока параметров и определение типа объекта
    void ROI_Area(Rect *one, Rect *two);
    void cv_sign_detector_boat(Mat *frame);      /// детектирование на основе сигнатуры кораблей
    void cv_point_detector();                    /// на характерных точках
    ~cv_detection_moving();
};


#endif // DETECTION_H
