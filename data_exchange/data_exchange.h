//
// Created by Nik Babuhin on 13.09.2021.
//

#ifndef MSD_FIRE_DATA_EXCHANGE_H
#define MSD_FIRE_DATA_EXCHANGE_H

#include <iostream>
#include <string>
#include <list>

#include <opencv2/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <curl/curl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <iomanip>


#include <mlx_90621/mlx90621.h>


#include "data_exchange/base64.h"

#include <nlohmann/json.hpp>

using namespace std;
using namespace cv;
using namespace nlohmann;


struct General
        {
    bool status_data = false;
    mutex data_server_mutex;
    //json_polygons::data_polygons data_polygon;
        };


class data_exchange{
private:
    char URL_event[100] = "http://192.168.2.26:4444/events";      /// информация по судну
    char URL_massege[100] = "http://192.168.2.26:4444/events";
public:
    data_exchange();
    void data_packetSend(char *packet);
    void data_packetFile(char *packet);
    void data_imgSend(int id, char *Pass, char *EventType, Mat frame, Mat frame_ir, int temp_max, int temp_min, int probability);
    void data_textSend(char *packet);
    void data_text();
    ~data_exchange();
};
class data_exchange_server
{
private:
    json j{};
    void parser_poligon();
public:
    General *general;
    json_polygons::data_polygons *DataPolygons{};
    explicit data_exchange_server(General *general_data){
        general = general_data;
        server();
    }
    void server();
    ~data_exchange_server();
};

#endif //MSD_FIRE_DATA_EXCHANGE_H
