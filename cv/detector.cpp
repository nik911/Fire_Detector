#include "fire_detector.h"

// Конструктор
FireDetector::FireDetector() {
    settings.type_subtractor = 1;
    switch (settings.type_subtractor) {
        case 0:
            pBackSub = createBackgroundSubtractorKNN();
            break;
        case 1:
            pBackSub = createBackgroundSubtractorMOG2();
            break;
        default:
            cout << "No type_subtract\n" << endl;
    }
    settings.test_mode = true;
    settings.min_area = 1500;
    settings.max_area = 100000;
    settings.time_ms = 1000 * 1;
}

// Деструктор
FireDetector::~FireDetector() {
    cout << "Stop detect" << endl;
}

// Основная функция детектирования
void FireDetector::detectFire(Mat *frame) {
    contours_detects.clear(); // Очищаем старые контуры
    frame_in = *frame;

    // Обновляем фоновую модель
    Mat frameOut = frame_in.clone();
    frameOut *= 1;

    processFrame(frame_in);
    pBackSub->apply(frame_in, fgMask);

    // Отделяем цвет сигнатуры движения
    inRange(fgMask, Scalar(60, 60, 60), Scalar(255, 255, 255), fgMask);

    // Применение алгоритма детектирования границ Кэнни
    Canny(fgMask, fgMask_ar, 10, 100, 3, true);

    // Поиск замкнутых контуров
    findContours(fgMask, contours_prev, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    Rect bounding_rect;
    contours_curr.clear();
    contours_detects.clear();

    for (size_t i = 0; i < contours_prev.size(); i++) {
        double area = contourArea(contours_prev[i]);
        if (area > settings.min_area && area < settings.max_area) {
            contours_curr.push_back(contours_prev[i]);
            contours_detects.push_back(i);
            bounding_rect = boundingRect(contours_prev[i]);
        }
    }

    count_largest_contour = contours_curr.size();

    if (count_largest_contour != 0) {
        trackObjects(frameOut);
    }

    IRService();
    if (event_ir1 == 1) {
        handleEvent(frameOut);
    }
}

// Обработка кадра
void FireDetector::processFrame(Mat &frame) {
    for (unsigned int y = 0; y < frame.rows; y++) {
        for (unsigned int x = 0; x < frame.cols; x++) {
            Vec3b &pixel = frame.at<Vec3b>(y, x);
            if (pixel[0] < 250 && pixel[1] < 250 && pixel[2] < 250) {
                if (pixel[0] > pixel[1] || pixel[1] > pixel[2] || pixel[0] > 200 || pixel[1] < 150 || pixel[2] < 200 ||
                    pixel[1] > pixel[2] - 10 || pixel[0] > pixel[1] || (pixel[0] < 240 && (pixel[0] > pixel[2] - 40 || pixel[0] > pixel[1] - 40)) ||
                    (pixel[0] >= 240 && (pixel[0] > pixel[2] || pixel[1] > pixel[2]))) {
                    pixel = Vec3b(0, 0, 0);
                }
            } else {
                if (pixel[0] > pixel[1] || pixel[1] > pixel[2] || pixel[0] > pixel[2]) {
                    pixel = Vec3b(0, 0, 0);
                }
            }
        }
    }
    blur(frame, frame, cv::Size(20, 20), cv::Point(-1, -1));
}

// Добавление нового объекта
void FireDetector::addNewFireObject(Mat &frameOut, Rect &bounding_rect, int centerX, int centerY) {
    FireObject new_fire;
    new_fire.counter++;
    new_fire.centerX.push_back(centerX);
    new_fire.centerY.push_back(centerY);
    new_fire.rect_FireData = bounding_rect;
    Mat img = frameOut(bounding_rect);
    img.copyTo(new_fire.frame_object);
    new_fire.tracking = true;
    new_fire.counter_time = 0;
    tracking_fire.push_back(new_fire);
}

// Обработка событий
void FireDetector::handleEvent(Mat &frameOut) {
    cout << "event!!!" << endl;
    int id = 1;
    string pas = "12345";
    string event_type = "FR";

    static uint32_t data[64 * sizeof(float)];
    Mat IR_mat(16, 4, CV_32FC1, data);

    int temp_max = 0, temp_min = 0, probabilit = 50;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 16; x++) {
            float val = data_ir1[16 * y + x];
            if (y == 0 && x == 0) {
                temp_min = val;
            }
            if (val > temp_max) {
                temp_max = val;
            }
            if (val < temp_min) {
                temp_min = val;
            }

            IR_mat.at<float>(x, y) = val;
        }
    }

    Mat normal_mat;
    rotate(IR_mat, IR_mat, ROTATE_90_CLOCKWISE);
    normalize(IR_mat, normal_mat, 0, 1.0, NORM_MINMAX, CV_32FC1);

    double minVal, maxVal;
    minMaxLoc(normal_mat, &minVal, &maxVal);
    Mat u8_mat;
    normal_mat.convertTo(u8_mat, CV_8U, 255.0 / (maxVal - minVal), -minVal);

    Mat size_mat;
    resize(u8_mat, size_mat, Size(160, 40), INTER_CUBIC);

    Mat falsecolor_mat;
    applyColorMap(size_mat, falsecolor_mat, COLORMAP_JET);

    char *Pass = const_cast<char *>(pas.c_str());
    char *EventType = const_cast<char *>(event_type.c_str());

    exchange.data_imgSend(id, Pass, EventType, frameOut, falsecolor_mat, temp_max, temp_min, probabilit);
}

// Расчет площади ROI
void FireDetector::ROI_Area(Rect *one, Rect *two) {
    three = *one & *two;
    if (three.area() > 0) {
        Area1 = one->area();
        Area2 = two->area();
        Area3 = three->area();
    }
}