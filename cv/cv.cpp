//
// Created by Nik Babuhin on 13.09.2021.
//

#include "cv.h"

cv_base::cv_base() {
    std::cerr << "CV:    Star cv_base!" << endl;
}

cv_base::~cv_base() {
    std::cerr << "CV:    Stop cv_base!" << endl;
}

void cv_general::cv(General *general)
{
    check_of_work();
    cv_detection_moving detectionMoving;
    // вычисление задержки на получение кадров
    double alltimes = 0.0;
    double count = 0;

    cam.read(frame);
    if (frame.empty()){
        cerr << "ERROR! получен пустой кадр \n";
    }

    // Наркатический припадок
    vector<Rect> boxes_cv;
    vector<int> indices_cv;

    int coint_no_frame = 0; /// счётчик пропущенных кадров
    while(!stop_flag_0)
    {
        int64 start = cv::getTickCount();
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        cam.read(frame);
        // В черно белом спектре
        //cvtColor(frame, frame, COLOR_BGR2GRAY);

        if (frame.empty()){
            cerr << "ERROR! получен пустой кадр \n"; /// костыль если не получисось связаться переподклюбчемся так как буффер при установке зон был переполнен
            cam.open(settings.URL);
            continue;
        }

        detectionMoving.cv_sign_detector_boat(&frame);
        /// проверка на получение данных
        /// отправка общих данны
        /*if (general->data_server_mutex.try_lock()) {
            if(general->status_data==1){
                /// запуск парсера данных
                gui.new_polygon(&frame, general->data_polygon);
                cout << "kek" << endl;
                /// до следующего получения данных
                general->status_data = false;
            }
            general->data_server_mutex.unlock();
        }*/
        /// продолжаем программу
        /// захреначим еще поток на сервер


        if(testMode.Test_m)
        {
            //usleep(30000); // 30 FPS
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration_cast<microseconds>( t2 - t1 ).count();
            alltimes += duration;
            count +=1;
            string label = format("Inference time for a frame : %.2f ms",duration/1000);
            putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
            label = format("FPS for a frame : %.2f fps", 1000/(duration/1000.0));
            putText(frame, label, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

            // отображение фрейма
            if(testMode.Frame)   imshow("Frame", frame);
            //if(testMode.FG_Mask) imshow("FG Mask", detect.fgMask);
            //if(testMode.Canny)   imshow("Canny", detect.fgMask_ar);

            if(waitKey(10) == 13)
            {
                double fps = cam.get(CAP_PROP_FPS);
                cout << "Frames per seconds : " << fps << endl;
                //cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;
            }
        }

        /// Рисовалка событий

        /*for (size_t i = 0; i < general_dnn->indices_roi.size(); ++i){
            int idx = general_dnn->indices_roi[i];
            Rect box = general_dnn->boxes_roi[idx];

            // box.width = box.x + box.width;
            // box.height = box.y + box.height;

            // cout<<"Before = " << box.x << " " << box.y << " " << box.width << " " << box.height << endl;

            // scale_coords(frame.size(), oldframe.size(), box);
            // cout<<"After = " << box.x << " " << box.y << " " << box.width << " " << box.height << endl;
            putText(gui.frame_roi, "boat", Point(box.x, box.y-10), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

            rectangle(gui.frame_roi, box, Scalar(255, 0, 0), LINE_4);
            //putText(frame, "boat", Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
            //drawPred(classIds[idx], confidences[idx], box.x, box.y, box.width, box.height, oldframe);
        }*/

        //кнопка закрытия
        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stoppig the video" << endl;
            break;
        }
    }
    // if (camera_thread.joinable())
    //    camera_thread.join();
}

cv_general::~cv_general()
{
    std::cout <<  "Closing CV\n" << endl;
}

void cv_general::check_of_work() {
    if(testMode.Test_m)
    {
        if(testMode.Frame)   namedWindow("Frame", WINDOW_KEEPRATIO);
        if(testMode.FG_Mask) namedWindow("FG Mask", WINDOW_KEEPRATIO);
        if(testMode.Canny)   namedWindow("Canny", WINDOW_KEEPRATIO);
    }
    /// VideoCapture.grab()
    cout << "Start camera capture!" << endl;
    if(settings.isState)
    {
        cam.open(0);
        namedWindow("Frame", WINDOW_KEEPRATIO);///WINDOW_GUI_NORMAL
        /*std::cout << "Camera:   URL" << endl;
        if(!cam.open(settings.URL))
        {
            namedWindow("Frame", WINDOW_KEEPRATIO);///WINDOW_GUI_NORMAL
            std::cout << "Camera Error. No data USB" << endl;
            cam.open(settings.message);
            cam >> frame;
            imshow("Frame", frame);
            // тут нужно вызвать сигнал на отправку ошибки серверу
            return;
        }else{
            cam.open(settings.URL);
        }*/
    }else {
        std::cout << "Camera:   Camera" << endl;
        if (!cam.open(settings.camIndex)) {
            namedWindow("Frame", WINDOW_KEEPRATIO);//
            std::cout << "Camera Error. No data URL" << endl;
            cam.open(settings.message);
            cam >> frame;
            imshow("Frame", frame);
            // тут нужно вызвать сигнал на отправку ошибки серверу
            return;
        } else {
            cam.open(settings.camIndex);
        }
    }

    double dWidth = cam.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cam.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    double dFPS = cam.get(CAP_PROP_FPS); //get the width of frames of the video


    cout << "Width:    "   << dWidth  << endl;
    cout << "Height:    "  << dHeight << endl;
    cout << "FPS:  "       << dFPS    << endl;
    cout << "Install CAM"  << endl;

    //cam.set(CAP_PROP_BUFFERSIZE, 5);   // установка буфера кадров на 10
}


