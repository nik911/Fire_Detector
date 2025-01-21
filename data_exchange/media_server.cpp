//
// Created by nikolai on 03.09.2021.
//

#include "data_exchange/media_server.h"

void media_server::server(General_media_server *media_server_general){

    ///  Стриминговый сервис
    //std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    MJPEGStreamer streamer;

    // По умолчанию для потоковой передачи используется 1 воркер
    // если вы хотите использовать 4 воркера:
    streamer.start(8080, 1);
    //streamer.start(8080);

    Mat frame_server;

    bool status_media = true;

    std::vector<uchar> buff_bgr;

    while (1){
        usleep(50000); // 30 FPS 5 fps
        if (media_server_general->media_server_mutex.try_lock_shared()) {
            /// запуск парсера данных
            //if(media_server_general->status_data){
            //    frame_server = media_server_general->frame.clone();
            //}
            //frame_server = media_server_general->frame.clone();
            if(media_server_general->status_data) {
                buff_bgr = media_server_general->buff_bgr;
            }
            /// до следующего получения данных
            status_media = true;
            media_server_general->status_data = false;

            media_server_general->media_server_mutex.unlock_shared();
        }

        if(status_media){
            /// Отправка кадров на импровезированный медиа сервер Http jmeg
            /// http://localhost:8080/bgr
            streamer.publish("/bgr", std::string(buff_bgr.begin(), buff_bgr.end()));
            status_media = false;
        }else{
           continue;
        }
    }
}

media_server::~media_server() {
    cerr << "Media_server close!" << endl;
}
