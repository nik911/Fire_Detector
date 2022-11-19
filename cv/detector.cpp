#include "cv/detector.h"


cv_detection_moving::cv_detection_moving()
{
    /// настроки детектора
    settings_sign.type_subtractor = 1;
    switch (settings_sign.type_subtractor)
    {
        case 0:
            pBackSub = createBackgroundSubtractorKNN();
            break;
            case 1:
                pBackSub = createBackgroundSubtractorMOG2();
                break;
                default:
                    cout << "No type_subtract\n" << endl;
    }
    settings_sign.Test_mode = true;
    settings_sign.min_area = 1500;/// 1500
    settings_sign.max_area = 100000;/// 1500
    settings_sign.time_ms = 1000*1;
}


cv_detection_moving::~cv_detection_moving()
{
    cout << "Stop detect" << endl;
}

void cv_detection_moving::cv_sign_detector(Mat *frame)
{
    event_ir1 = 0;
    contours_detects.clear(); // очищаем старые контуры
    frame_in = *frame;
    /// обновляем фоновую модель
    Mat frameOut = frame_in.clone();
    frameOut *= 1;
    for(unsigned int y=0;y < frame_in.rows;y++)
    {
        for(unsigned int x=0;x < frame_in.cols;x++)
        {
            if((frame_in.at<Vec3b>(y,x)[0] < 250)&&(frame_in.at<Vec3b>(y,x)[1] < 250)&&(frame_in.at<Vec3b>(y,x)[2] < 250)){
                if((frame_in.at<Vec3b>(y,x)[0])>(frame_in.at<Vec3b>(y,x)[1])){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if((frame_in.at<Vec3b>(y,x)[1])>(frame_in.at<Vec3b>(y,x)[2])){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[0]>200){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[1]<150){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[2]<200){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[1]>frame_in.at<Vec3b>(y,x)[2]-10){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[0]>frame_in.at<Vec3b>(y,x)[1]){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if(frame_in.at<Vec3b>(y,x)[0]<240){
                    if(frame_in.at<Vec3b>(y,x)[0]>frame_in.at<Vec3b>(y,x)[2]-40){
                        frame_in.at<Vec3b>(y,x)[0] = 0;
                        frame_in.at<Vec3b>(y,x)[1] = 0;
                        frame_in.at<Vec3b>(y,x)[2] = 0;
                    }
                    if(frame_in.at<Vec3b>(y,x)[0]>frame_in.at<Vec3b>(y,x)[1]-40){
                        frame_in.at<Vec3b>(y,x)[0] = 0;
                        frame_in.at<Vec3b>(y,x)[1] = 0;
                        frame_in.at<Vec3b>(y,x)[2] = 0;
                    }
                }else{
                    if(frame_in.at<Vec3b>(y,x)[0]>frame_in.at<Vec3b>(y,x)[2]){
                        frame_in.at<Vec3b>(y,x)[0] = 0;
                        frame_in.at<Vec3b>(y,x)[1] = 0;
                        frame_in.at<Vec3b>(y,x)[2] = 0;
                    }
                    if(frame_in.at<Vec3b>(y,x)[1]>frame_in.at<Vec3b>(y,x)[2]){
                        frame_in.at<Vec3b>(y,x)[0] = 0;
                        frame_in.at<Vec3b>(y,x)[1] = 0;
                        frame_in.at<Vec3b>(y,x)[2] = 0;
                    }
                }
            }else{
                if((frame_in.at<Vec3b>(y,x)[0])>(frame_in.at<Vec3b>(y,x)[1])){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if((frame_in.at<Vec3b>(y,x)[1])>(frame_in.at<Vec3b>(y,x)[2])){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
                if((frame_in.at<Vec3b>(y,x)[0])>(frame_in.at<Vec3b>(y,x)[2])){
                    frame_in.at<Vec3b>(y,x)[0] = 0;
                    frame_in.at<Vec3b>(y,x)[1] = 0;
                    frame_in.at<Vec3b>(y,x)[2] = 0;
                }
            }
        }
    }
    //imshow("COLOR1", frame_in);
    //inRange(frame_in, Scalar(255, 0, 0), Scalar(255, 255, 255), frame_in); /// 60
    //imshow("COLOR2", frame_in);
    blur(frame_in, frame_in, cv::Size(20, 20),  cv::Point(-1, -1));//7 добавим немного размытия чтобы убрать мелкий дребезг
    pBackSub->apply(frame_in, fgMask);

    /// отделяем цвет сигнатуры движения
    inRange(fgMask, Scalar(60, 60, 60), Scalar(255, 255, 255), fgMask);
    //imshow("InRange", fgMask);
    /// применение алгоритма детектирования границ Кэнни
    ///threshold(fgMask, fgMask, 60, 255, THRESH_OTSU);
    /// Поиск контуров
    Canny(fgMask, fgMask_ar, 10, 100, 3, true);  /// 10 100 3
    /// поиск замкнутых контуров
    findContours(fgMask, contours_prev, RETR_CCOMP, CHAIN_APPROX_SIMPLE); //RETR_FLOODFILL RETR_CCOMP
    //imshow("FG Mask", fgMask);
    Rect bounding_rect;
    contours_curr.clear();
    contours_detects.clear();
    for (size_t i = 0; i < contours_prev.size(); i++)  // выполняем перебор каждого контура
    {
        double area = contourArea(contours_prev[i]);  //присваиваем вес контура
        if (800 < settings_sign.min_area < 10000) { //&& (area < settings_sign.max_area)
            contours_curr.push_back(contours_prev[i]);
            contours_detects.push_back(i);             // находим сответствующие контуры
            bounding_rect = boundingRect(contours_prev[i]);
            //rectangle(frameOut, bounding_rect, Scalar(0, 255, 0), 2);
        }
    }
    count_largest_contour = contours_curr.size();  // счетчик текущих контуров
    if(count_largest_contour != 0){
        vector<int> new_object;   /// для добаления новых объектов
        vector<int> index_contours;  /// для добаления новых объектов
        /// Алгоритм трекинга базовый
        for(unsigned int i1 = 0; i1 < contours_curr.size(); i1++) {
            bounding_rect = boundingRect(contours_curr[i1]);
            /// Находим цент контура
            centerX = abs(bounding_rect.width / 2 + bounding_rect.x);
            centerY = abs(bounding_rect.height / 2 + bounding_rect.y);
            //cerr << "tyt1" << endl;

            if(tracking_fire.empty() && !contours_curr.empty()){
                Mat img;
                bounding_rect = boundingRect(contours_curr[i1]);
                /// Находим цент контура
                centerX = abs(bounding_rect.width / 2 + bounding_rect.x);
                centerY = abs(bounding_rect.height / 2 + bounding_rect.y);

                fire_object new_fire;

                new_fire.counter++;
                new_fire.centerX.push_back(centerX);
                new_fire.centerY.push_back(centerY);
                new_fire.rect_FireData = Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height);
                img = frameOut(Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height));
                img.copyTo(new_fire.frame_object);
                new_fire.traking = true;
                new_fire.counter_time = 0;

                tracking_fire.push_back(new_fire);
                //cerr << "kekus" << endl;
            }else if(!tracking_fire.empty()){
                for (unsigned int s = 0; s < tracking_fire.size(); s++) {
                    /// Вычисление расстояние от объекта до контура по центу контура и центру объектов
                    double trek_dist = abs(sqrt((centerX - tracking_fire[s].centerX.back()) *
                            (centerX - tracking_fire[s].centerX.back()) +
                            (centerY - tracking_fire[s].centerY.back()) *
                            (centerY - tracking_fire[s].centerY.back())));

                    /// Поиск промежуточного ROI по наложению и вычисление расстояния от 1 и 2 ROI
                    ROI_Area(&tracking_fire[s].rect_FireData, &bounding_rect);

                    double I = abs(Area3 / (Area1 + Area2 - Area3)); /// площадь пересечения
                    //cerr << I << endl;
                    /// Механизм обтработки только тех замкнутых контуров которые пересекаются с объектами
                    if((I > 0)&&(trek_dist < 50)){
                        //cerr << "kek" << endl;
                        tracking_fire[s].counter++;
                        tracking_fire[s].centerX.push_back(centerX);
                        tracking_fire[s].centerY.push_back(centerY);
                        tracking_fire[s].rect_FireData = Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height);
                        boat_img = frameOut(Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height));
                        boat_img.copyTo(tracking_fire[s].frame_object);
                        tracking_fire[s].traking = true;
                        tracking_fire[s].counter_time = 0;
                        tracking_fire[s].area.push_back(Area1);
                        //cerr << "trak" << endl;
                    }else{
                        /// На создаватор объектов
                        new_object.push_back(s);
                        index_contours.push_back(i1);
                        //cerr << "tyt2" << endl;
                    }
                    //cerr << "tyt2" << endl;
                }
            }
        }

        for (unsigned int s = 0; s < new_object.size(); s++) {
            int i = count(new_object.begin(), new_object.end(), new_object[s]);
            if (i == contours_curr.size()){
                tracking_fire[new_object[s]].traking = false;
            }
        }

        int index = 111111;

        for(unsigned int i1 = 0; i1 < index_contours.size(); i1++) {
            if(new_object.size()==0){
                break;
            }
            int i = count(new_object.begin(), new_object.end(), index_contours[i1]);

            //cerr << i << endl;
            if (i == tracking_fire.size() && index == 111111){

                index = i1;
                //cerr << "tyt3" << endl;

                Mat img;
                bounding_rect = boundingRect(contours_curr[index_contours[i1]]);
                /// Находим цент контура
                centerX = abs(bounding_rect.width / 2 + bounding_rect.x);
                centerY = abs(bounding_rect.height / 2 + bounding_rect.y);

                fire_object new_fire;

                new_fire.counter++;
                new_fire.centerX.push_back(centerX);
                new_fire.centerY.push_back(centerY);
                new_fire.rect_FireData = Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height);
                img = frameOut(Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height));
                img.copyTo(new_fire.frame_object);
                new_fire.traking = true;
                new_fire.counter_time = 0;

                tracking_fire.push_back(new_fire);
                //cerr << "tyt2" << endl;
            }else if (i == tracking_fire.size() && index != i){
                index = i1;
                //cerr << "tyt3" << endl;

                Mat img;
                bounding_rect = boundingRect(contours_curr[index_contours[i1]]);
                /// Находим цент контура
                centerX = abs(bounding_rect.width / 2 + bounding_rect.x);
                centerY = abs(bounding_rect.height / 2 + bounding_rect.y);

                fire_object new_fire;

                new_fire.counter++;
                new_fire.centerX.push_back(centerX);
                new_fire.centerY.push_back(centerY);
                new_fire.rect_FireData = Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height);
                img = frameOut(Rect(bounding_rect.x, bounding_rect.y, bounding_rect.width, bounding_rect.height));
                img.copyTo(new_fire.frame_object);
                new_fire.traking = true;
                new_fire.counter_time = 0;

                tracking_fire.push_back(new_fire);
            }
        }

        for (auto & s : tracking_fire) {
            if(!s.traking){
                s.counter_time++;
            }
        }
        bool event = false;
        /// Удалятор
        for (unsigned int s = 0; s < tracking_fire.size(); s++) {
            if(tracking_fire[s].counter_time > 5)
            {
                //cerr << "tyt3" << endl;
                tracking_fire.erase(tracking_fire.begin() + s);  // удаление объекта
            }
        }
        /// Рисуем и Отправляем
        for (unsigned int s = 0; s < tracking_fire.size(); s++) {
            if(tracking_fire[s].counter > 60)
            {
                //event = true;
                cerr << "tyt4" << endl;
                for(double t : tracking_fire[s].area){
                    cout << t << endl;
                }
                double summ_area = 0, nom = 0;
                bool kek = false;
                vector<double> nom_v;
                nom = tracking_fire[s].area[0];
                for (auto& n : tracking_fire[s].area){
                    summ_area += n;
                    if(!kek){
                        kek = true;
                        nom_v.push_back(abs(nom+n));
                    }else{
                        nom_v.push_back(abs(nom-n));
                    }
                }
                double sum_med = summ_area/60;
                for (auto& n : nom_v){
                    summ_area += n;
                }
                double sum_med_sk = summ_area/60;
                int count_z = 0;

                bool b = false;
                for (auto& n : tracking_fire[s].area){
                    //if((sum_med-3*sum_med_sk) < n < (sum_med+3*sum_med_sk)) count_z++;
                    if(sum_med > n && !b){
                        b=true;
                        count_z++;
                    }else if(sum_med < n && b){
                        b=false;
                        count_z++;
                    }
                }
                count_z = count_z/2;
                cerr << "2PI: ----" << count_z << endl;
                if ((count_z <= 30) && (count_z >= 4)){
                    event = true;
                }
                putText(frameOut, "FIRE!", Point(tracking_fire[s].rect_FireData.x, tracking_fire[s].rect_FireData.y),
                        FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
                rectangle(frameOut, tracking_fire[s].rect_FireData, Scalar(0, 0, 255), 2, LINE_4, 0);
                tracking_fire.erase(tracking_fire.begin() + s);  // удаление объекта
            }
        }



        cout << "kek" << endl;
        IRService();
        if (event_ir1 == 1){
            event = true;
        }

        if(event){
            //cout << tracking_fire.size() << endl;
            event = false;
            cout << "event!!!" << endl;
            int id = 1;


            /// Получение данных от температурной маски
           // IRService_vd();


            /*std::time_t t = std::time(NULL);   // get time now
            std::tm* now = std::localtime(&t);
            ostringstream oss;
            int y = (now->tm_year), mo = (now->tm_mon), d = (now->tm_mday), h =(now->tm_hour), m =(now->tm_sec), s = (now->tm_sec);
            //cout <<  "TIME_" << h << ":" << m << ":" << s << endl;
            cout <<  "TIME_" << d << ":" << mo << ":" << y << " " << h << ":" << m << ":" << s << endl;
            oss << d << ":" << mo << ":" << y << " " << h << ":"<< m << ":" << s; ///<< h << ":"<< m << ":" << s ТУТА ВРЕМЯ
            basic_string<char, char_traits<char>, allocator<char>> buf_time = oss.str();*/

            string pas = "12345";
            string event_type = "FR";


            char *Pass = const_cast<char *>(pas.c_str());
            char *EventType = const_cast<char *>(event_type.c_str());
            //char *data_time = const_cast<char *>(buf_time.c_str());
            //char *Data = const_cast<char *>(it1->name_detection_zone.c_str());
            exchange.data_imgSend(id, Pass, EventType, frameOut);
            //imshow("Event", frameOut);
        }

        /// Отправляем постоянно через промежуток времени
        index_contours.clear();
        new_object.clear();
    }
}

void cv_detection_moving::ROI_Area(Rect *one, Rect *two)
{
    three = *one & *two;
    if((three.x>0)&&(three.y>0)&&(three.width>0)&&(three.height>0))
    {
        Area1 = abs((one->x-one->width)*(one->y-one->height));
        Area2 = abs((two->x-two->width)*(two->y-two->height));
        Area3 = abs((three.x-three.width)*(three.y-three.height));
    }
}

//
// Created by Nik Babuhin on 13.09.2021.
//

