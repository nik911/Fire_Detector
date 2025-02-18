#include "data_exchange/data_exchange.h"

float data_ir[64];
float data_ir1[64];
int event_ir1;

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
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPIDLE, 5L);/// Время ожидания 120 сек
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPINTVL, 30L);/// Проверка активности 60 сек

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

void data_exchange::data_imgSend(int id, char *Pass, char *EventType, Mat frame, Mat frame_ir, int temp_max, int temp_min, int probability) {
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
    char buf_img[30000];
    strcpy(buf_img, encoded.c_str());

    frame_ir.copyTo(img);
    quality_params = vector<int>(2);              // Вектор параметров качества сжатия
    quality_params[0] = 1; // Кодек JPEG
    quality_params[1] = 20;
    imencode(".jpg",img, buf, quality_params);
    encoded = base64_encode(enc_msg, buf.size());
    char ir_buf_img[30000];
    strcpy(ir_buf_img, encoded.c_str());


    struct timeval tv{};
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    strftime (time_string, sizeof (time_string), "%d.%m.%Y %H:%M:%S", ptm);


    std::ostringstream buffer_string;
    buffer_string << "{ \"id\": " << id
                  << ", \"id_psw\": \"" << Pass
                  << "\", \"event_type\": \"" <<  EventType
                  << "\", \"date_time\": \"" << time_string
                  << "\", \"base64_frame\": \"" << buf_img
                  << "\", \"base64_ir\": \"" << ir_buf_img
                  << "\", \"temp_max\": " << temp_max
                  << ", \"temp_min\": " << temp_min
                  << ", \"probability\": " << probability
                  << "}";


    string out_str;
    out_str = buffer_string.str();
    cout << out_str << endl;

    data_packetSend(const_cast<char*>(out_str.c_str()));
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

void data_exchange_server::parser_poligon() {
    // Отправка команды на поворот установки
}

void data_exchange_server::server() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[30000];
    struct sockaddr_in serv_addr{}, cli_addr{};
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

    while(true){
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

data_exchange_server::~data_exchange_server() {
    cerr << "Data_exchange_server bay!bay!";
}