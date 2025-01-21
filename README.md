# Учебный проект для проведения занятия по MVP-разработке
# Детектор огня с использованием камеры и инфракрасного датчика

### Установка библиотек
>Выполните в терминале
```
* Sistem
sudo apt-get update -y
sudo apt-get install modemmanager* udev udev-discover usb-modeswitch* libusb-1.0-0* network-manager*
sudo apt-get install ppp usb-modeswitch modemmanager -y
sudo tar -xzvf /usr/share/usb_modeswitch/configPack.tar.gz 
sudo apt-get install modem-manager-gui -y

sudo apt-get install pptp-linux
sudo apt-get install pptpd

sudo apt-get install cmake cmake-gui -y

sudo apt-get install libboost-all-dev -y
sudo apt-get install libcurl4-gnutls-dev -y
sudo apt-get install libcurl4-openssl-dev -y
sudo apt-get install libcurl4-nss-dev -y

sudo apt-get install i2c-tools -y
```
### Установка nadjieb_mjpeg_streamer
```
git clone https://github.com/nadjieb/cpp-mjpeg-streamer.git
```


### Установка opencv
```
sudo apt install libeigen3-dev -y
sudo apt-get install libgtk2.0-dev libgtk-3-dev -y
sudo apt-get install gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio -y
sudo apt install libgstreamer1.0–0 libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev -y
sudo apt-get install -y libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavresample-dev -y
sudo apt-get install pkg-config -y
sudo apt install libdc1394–22-dev -y
sudo apt-get install libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev -y
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev -y

git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
```

### Установка nadjieb_mjpeg_streamer
```
git clone https://github.com/nadjieb/cpp-mjpeg-streamer.git
mkdir build && cd build
cmake .. && make
sudo make install
```

### Запуск проекта
```
cd bin
./Fire_Detector
```

### Подготовка VNC для удобной работы
```
https://pcminipro.ru/nastrojka-serverov/ustanovka-i-nastrojka-vnc-servera-tightvncserver-dlya-udalennogo-upravleniya-sistemoj-s-ispolzovaniem-graficheskoj-sredy/
```
