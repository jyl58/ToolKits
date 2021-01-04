/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include <stdio.h>
#include "HttpClientInterface.hpp"
#include "LogInterface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
const std::string LOG_TAG="main";
extern "C" void app_main(){
	HttpClientInterface::getHttpClientInstance();
	while(1){
		LOGI(LOG_TAG.c_str(),"helle word\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}