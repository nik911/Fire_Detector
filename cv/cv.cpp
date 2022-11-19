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
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};

    check_of_work();
    cv_detection_moving detectionMoving;
    // вычисление задержки на получение кадров
    double alltimes = 0.0;
    double count = 0;

    cam.read(frame);
    if (frame.empty()){
        cerr << "ERROR! получен пустой кадр \n";
    }

    /// Наркатический припадок
    vector<Rect> boxes_cv;
    vector<int> indices_cv;

    /// Поток на медиа сервер
    auto *media_server_general = new General_media_server;
    thread thread_1(media_server_img, ref(media_server_general));

    int coint_no_frame = 0; /// Cчётчик пропущенных кадров
    while(!stop_flag_0)
    {
        int64 start = cv::getTickCount();
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        cam.read(frame);

        if (frame.empty()){
            cerr << "ERROR! получен пустой кадр \n"; /// костыль если не получисось связаться переподклюбчемся так как буффер при установке зон был переполнен
            cam.open(settings.URL);
            continue;
        }
        rotate(frame, frame, ROTATE_180);

        media_server_general->media_server_mutex.lock_shared();
        if(!media_server_general->status_data) {
            cv::imencode(".jpg", frame, media_server_general->buff_bgr, params);
            media_server_general->status_data = true;
        }
        media_server_general->media_server_mutex.unlock();  ///// была хуйня

        /// запускаем cv обработку
        detectionMoving.cv_sign_detector(&frame);


        if(testMode.Test_m)
        {
            //usleep(30000); // 30 FPS
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration_cast<microseconds>( t2 - t1 ).count();
            alltimes += duration;
            count +=1;
            string label = format("Inference time for a frame : %.2f ms",duration/1000);
            //putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
            label = format("FPS for a frame : %.2f fps", 1000/(duration/1000.0));
            cout << label << endl;
            //putText(frame, label, Point(0, 35), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

            // отображение фрейма
            //if(testMode.Frame)   imshow("Frame", frame);
            //if(testMode.FG_Mask) imshow("FG Mask", detect.fgMask);
            //if(testMode.Canny)   imshow("Canny", detect.fgMask_ar);

            if(waitKey(10) == 13)
            {
                double fps = cam.get(CAP_PROP_FPS);
                cout << "Frames per seconds : " << fps << endl;
                //cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;
            }
        }

        //кнопка закрытия
        if (waitKey(10) == 27)
        {
            cout << "Esc key is pressed by user. Stoppig the video" << endl;
            break;
        }
    }
    thread_1.join();
    //streamer.stop();
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
    }else {
        std::cout << "Camera:   Camera" << endl;
        if (!cam.open(settings.camIndex)) {
            namedWindow("Frame", WINDOW_KEEPRATIO);//
            std::cout << "Camera Error. No data URL" << endl;
            cam.open(settings.message);
            cam >> frame;
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
}


void media_server_img(General_media_server *general_media_server){
    media_server m_server(general_media_server);
}