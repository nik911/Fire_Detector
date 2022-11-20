#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>

#include <pthread.h>

#include "cv/cv.h"
#include "data_exchange/data_exchange.h"

#include <unistd.h>


using namespace std;


void data_server(General *general){
    data_exchange_server server_data(general);
}

void general_cv(General *general){
    /// добавить жеткий перезапуск при получении данных
    cv_general cv_general(general);
}

int main(int argc, char *argv[])
{
    auto *general = new General;

   // thread thread_1(data_server, ref(general));
    thread thread_2(general_cv, ref(general));

   // thread_1.join();
    thread_2.join();
    return 0;
}
