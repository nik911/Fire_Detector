#include "data_exchange/data_exchange.h"



data_exchange::data_exchange()
{
    cerr << "Start data_exchange!" << endl;
}

data_exchange::~data_exchange()
{
    cerr << "Stop data_exchange!" << endl;
}

/////////////////////////////////////////
///Функция отвечает за отправку пакета
//////////////////////////////////////////////
void data_exchange::data_packetSend(char *packet)
{
    CURL *hnd;
    curl_slist *slist1;

    slist1 = nullptr;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");

    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_URL, URL_event);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, packet);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.72.0");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    curl_slist_free_all(slist1);
}

void data_exchange::data_packetFile(char *packet)
{
    CURL *curl;
    //CURLcode res;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL_massege);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = nullptr;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filedata(part, packet);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_perform(curl);
        curl_mime_free(mime);
    }
    curl_easy_cleanup(curl);
}

void data_exchange::data_imgSend(int id, char *Pass, char *EventType, Mat frame) {
    /////////////////////////////////////////////////
    //Перекодировка картинки нового корабля в Base64
    ///////////////////////////////////////////////////
    Mat img;
    frame.copyTo(img);
    std::vector<uchar> buf;
    vector<int> quality_params = vector<int>(2);              // Вектор параметров качества сжатия
    quality_params[0] = 1; // Кодек JPEG
    quality_params[1] = 20;
    imencode(".jpg",img, buf, quality_params);
    auto *enc_msg = reinterpret_cast<unsigned char*>(buf.data());
    string encoded = base64_encode(enc_msg, buf.size());
    //cout << encoded << endl;

    //memset(buf,0,sizeof(buf));
    //sprintf(buf,"/media/nikolai/Новый том/VF_portable/VF_tracking/archive/%s.jpg", buf_name.c_str());
    ////////////////////////////////////////////////////////
    //Отправка пакета если новый корабль
    ////////////////////////////////////////////////////////////
    char buffer[40000];
    char buf_img[30000];
    strcpy(buf_img, encoded.c_str());
    memset(buffer,'\0', sizeof(buffer));
    //sprintf(buffer, R"({"name":"%s", "object":"%s", "img":":%s"})", name, zona, buf_img);


    //string data_time = "16.01.2021 9:44:26";
    //char *time_data = const_cast<char *>(data_time.c_str());



    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    strftime (time_string, sizeof (time_string), "%d.%m.%Y %H:%M:%S", ptm);

    sprintf(buffer, R"({"Id":%i, "Pass":"%s", "EventType":"%s", "Date":"%s", "Base64":"%s"})", id, Pass, EventType, time_string, buf_img);
    //sprintf(buffer, R"({"name":"kek1", "object":"kek2", "img":"kek3"})");
    cout << buffer << endl;
    data_packetSend(buffer);
    //cout << buffer << endl;
}


void data_exchange::data_textSend(char *packet) {

}

void data_exchange::data_text() {
    /////////////////////////////////////////////////
    //Перекодировка картинки нового корабля в Base64
    ///////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    //Отправка пакета если новый корабль
    ////////////////////////////////////////////////////////////
    char buffer[30000];
    memset(buffer,'\0', sizeof(buffer));
    //sprintf(buffer, R"({"name":"%s", "object":"%s", "img":":%s"})", name, zona, buf_img);
    sprintf(buffer, R"({"name":"kek1", "object":"kek2", "img":"kek3"})");
    cout << buffer << endl;
    data_packetSend(buffer);
    //cout << buffer << endl;
}


/*
void data_exchange_server::parser_poligon() {
    // convert from JSON: copy each value from the JSON object
    json_polygons::data_polygons dataPolygons{
        j["portId"].get<int>(),
        j["cameraId"].get<int>(),
        j["polygons"].get<list<json>>()
    };

    for (json it1: dataPolygons.polygons) {
        cout << it1 << endl;
        json_polygons::point_polygons polyg{
            it1["id"].get<int>(),
            it1["points"].get<list<json>>()
        };

        for (json it2: polyg.points) {
            cout << it2 << endl;
            json_polygons::point_polygon p2{
                it2["id"].get<int>(),
                it2["x"].get<double>(),
                it2["y"].get<double>()
            };
            polyg.point.push_back(p2);
        }
        dataPolygons.polygons_obj.push_back(polyg);
    }
    /// присваиваем сформированную структуру
    //if (general->data_server_mutex.try_lock()) {

    // test data parser
    cout << "Первый уровень вложения порт:   " << dataPolygons.portId << endl;
    cout << "Первый уровень вложения камера: " << dataPolygons.cameraId << endl;
    cout << "копнем глубже:   " << dataPolygons.polygons_obj[0].id << endl;
    // копнем глубже
    cout << "копнем глубже:   " << dataPolygons.polygons_obj[0].point[0].x << endl;

    //}
    //general->data_server_mutex.unlock();
    if (general->data_server_mutex.try_lock()) {
        //general->data_server_mutex.lock();
        general->data_polygon = dataPolygons;
        general->status_data = true;
        general->data_server_mutex.unlock();
    }
}
*/

/*
void data_exchange_server::server() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[30000];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cerr << "ERROR opening socket";
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        cerr << "ERROR on binding";

    listen(sockfd, SOMAXCONN);
    clilen = sizeof(cli_addr);

    while(1){
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                                   &clilen);
        if (newsockfd < 0)
            cerr << "ERROR on accept";
        bzero(buffer,30000);
        n = read(newsockfd, buffer,30000);

        if (n < 0) cerr << "ERROR reading from socket";
        printf("Here is the message: %s\n",buffer);


        n = write(newsockfd,"I got your message",18);
        if (n < 0) cerr << "ERROR writing to socket";

        std::string str;
        bool kek = false;
        for (auto& element : buffer) {
            if(element == '{'){ kek = true; }
            if(kek){
                str.push_back(element);
            }
        }
        std::cout << str << '\n';

        if(kek||(str.size()>0)){
            j = json::parse(str);
            parser_poligon();
            std::cout << j << std::endl;
        }
    }
    close(newsockfd);
    close(sockfd);
}
*/
/*
data_exchange_server::~data_exchange_server() {
    cerr << "Data_exchange_server bay!bay!";
}*/

