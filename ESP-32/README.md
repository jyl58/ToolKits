# AIBox_RTOS_ESP32

基于FreeRTOS系统的智能音箱（D2）项目。
1.目前支持的语音方案由百度提供；
2.WiFi蓝牙模块采用乐鑫ESP32模组。
整个系统运行在ESP32模组的MCU。

编译：
D2目前支持cmake构建编译。
git clone http://10.18.203.116/lihan9/AIBox_RTOS_ESP32.git
1)D2编译
cd AIBox_RTOS_ESP32/AiBox
mkdir build
cd build
cmake ..
make -j4

2)测试编译
在AIBox_RTOS_ESP32/test文件夹下新建一个测试的子文件夹,如,test_xxx
按照已有的测试例程完善main函数及CmakeLists.txt。
mkdir build
cd build
cmake ..
make -j4

下载固件到flash
1）命令行操作：
 python $ADF_PATH/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyS3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xd000 ./build/ota_data_initial.bin 0x1000 ./build/bootloader/bootloader.bin 0x10000 ./build/dul1906.bin 0x8000 ./build/partitions.bin 0x490000 ./firmware/cupid_slave_v1.4.0.C.bin 0x7fe000 profiles/profile.bin 0x7e0000 tone/audio-esp.bin
 
 将对应串口，文件目录改到实际用到的。
2）界面操作见 乐鑫固件下载软件使用