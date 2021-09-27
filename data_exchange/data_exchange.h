//
// Created by Nik Babuhin on 13.09.2021.
//

#ifndef MSD_FIRE_DATA_EXCHANGE_H
#define MSD_FIRE_DATA_EXCHANGE_H

#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
//include <opencv2/xfeatures2d.hpp>
#include <opencv2/video/background_segm.hpp>
//#include <opencv2/bgsegm.hpp>

#include <curl/curl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#include <mlx_90621/mlx90621.h>


#include "data_exchange/base64.h"

//#include <nlohmann/json.hpp>

using namespace std;
using namespace cv;
//using namespace nlohmann;


/*namespace json_polygons{
    struct point_polygon{
        int id;
        double x;
        double y;
    };
    struct point_polygons{
        int id;
        list<json> points;
        vector<point_polygon> point;
    };
    // главный список внутри него все
    struct data_polygons{
        int portId;
        int cameraId;
        list<json> polygons;
        vector<point_polygons> polygons_obj;
    };
}*/


struct General
        {
    bool status_data = false;
    mutex data_server_mutex;
    //json_polygons::data_polygons data_polygon;
        };


class data_exchange


{
private:
    char URL_event[100] = "http://192.168.250.81:90/DataRecipient/GetStringDate";      /// информация по судну
    //char URL_event[100] = "http://192.168.250.81:90/DataRecipient/GetStringDate";
    //char URL_event[100] = "http://192.168.250.119:8000/";
    char URL_massege[100] = "http://192.168.250.225:8080/";
    //char URL_event[100] = "http://192.168.250.183:8080/api/boats";      /// информация по судну
    //char URL_massege[100] = "http://192.168.250.183:8080//api/boats";
public:
    data_exchange();
    void data_packetSend(char *packet);
    void data_packetFile(char *packet);
    void data_imgSend(int id, char *Pass, char *EventType, Mat frame);
    void data_textSend(char *packet);
    void data_text();
    ~data_exchange();
};
/*class data_exchange_server
        {
        private:
            json j;
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
        };*/

#endif //MSD_FIRE_DATA_EXCHANGE_H
