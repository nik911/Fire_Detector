#ifndef MSD_FIRE_DETECTOR_H
#define MSD_FIRE_DETECTOR_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <vector>
#include <numeric> // для std::accumulate

#include "data_exchange/data_exchange.h"
#include "mlx_90621/mlx90621.h"

using namespace cv;
using namespace std;

// Класс для хранения информации об объекте
class FireObject {
public:
    unsigned int counter = 0;         // Счетчик существования объекта
    unsigned int counter_time = 0;    // Счетчик для удаления
    bool tracking = false;            // Флаг отслеживания
    Mat frame_object;                 // Кадр с объектом
    Rect rect_FireData;               // Прямоугольник, описывающий объект
    vector<double> area;              // Площади объекта
    vector<double> centerX;           // Координаты X центра объекта
    vector<double> centerY;           // Координаты Y центра объекта
};

// Настройки детектора
class SettingsDetector {
public:
    int type_subtractor = 0;          // Тип субтрактора фона
    double min_area = 1500;           // Минимальная площадь контура
    double max_area = 10000000;       // Максимальная площадь контура
    bool test_mode = true;            // Режим тестирования
    int time_ms = 0;                  // Время задержки
};

// Класс для детектирования подвижных объектов
class FireDetector {
private:
    // ROI Area
    double Area1 = 0, Area2 = 0, Area3 = 0;
    Rect three;
    double centerX = 0, centerY = 0;
    DataExchange exchange;
    Mat frame_out, boat_img;

    // Параметры и настройки
    bool state_img = true;
    Mat frame_in, fgMask, fgMask_ar;
    SettingsDetector settings;
    Ptr<BackgroundSubtractor> pBackSub;

    // Вспомогательные функции
    void processFrame(Mat &frame);
    void trackObjects(Mat &frameOut);
    void addNewFireObject(Mat &frameOut, Rect &bounding_rect, int centerX, int centerY);
    void trackExistingFireObjects(Mat &frameOut, Rect &bounding_rect, int centerX, int centerY, vector<int> &new_object, vector<int> &index_contours);
    void handleNewObjects(Mat &frameOut, vector<int> &new_object, vector<int> &index_contours);
    void removeInactiveObjects();
    void drawAndSendEvents(Mat &frameOut);
    void handleEvent(Mat &frameOut);

public:
    vector<FireObject> tracking_fire; // Вектор объектов с огнем
    vector<vector<Point>> contours_prev, contours_curr; // Вектора контуров
    int count_contour = 0;            // Счетчик текущих контуров
    int count_largest_contour = 0;
    vector<int> contours_detects;     // Индексы соответствующих контуров

    FireDetector();                   // Конструктор
    void ROI_Area(Rect *one, Rect *two);
    void detectFire(Mat *frame);      // Основная функция детектирования
    ~FireDetector();                  // Деструктор
};

// Внешние функции
extern "C" void IRService();
extern "C" void IRService_vd();

#endif // MSD_FIRE_DETECTOR_H