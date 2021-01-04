/****************************************
*   uart esp32 class for .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.09
*****************************************/
#include <cstring>
#include "esp_log.h"
#include "esp_console.h"
#include "AiBox_UARTESP32.h"
#include "AiBox_UART.h"
using namespace AiBox;
const std::string LOG_TAG="ESP32UART";
static int uartSendCommand(int argc, char **argv)
{
    if(argc!=2){
        ESP_LOGI(LOG_TAG.c_str(),"Command need 2 param!!");
        return 0;
    }
    std::string command=argv[1];
    SystemUart::getSystemUartInstance()->write(command);
    return 0;
}
void 
ESP32Uart::registerUartCMD()
{
    //uart cmd
	esp_console_cmd_t join_cmd;
    join_cmd.command = "uartsend";
    join_cmd.help = "uart send command";
    join_cmd.hint = NULL;
    join_cmd.func = &uartSendCommand;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
bool 
ESP32Uart::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Exec command uart init.");
    
    //step1: config the uart1
    uart_config_t uart_config;
    memset(&uart_config,0,sizeof(uart_config));
    uart_config.baud_rate=115200;
    uart_config.data_bits=UART_DATA_8_BITS;
    uart_config.flow_ctrl=UART_HW_FLOWCTRL_DISABLE;
    uart_config.parity= UART_PARITY_DISABLE;
    //uart_config.rx_flow_ctrl_thresh
    uart_config.stop_bits=UART_STOP_BITS_1;
    if(uart_param_config(COMMAND_UART_NUM, &uart_config)!=ESP_OK){
        return false;
    }
    //step2: set the uart1 gpio
    uart_set_pin(COMMAND_UART_NUM,COMMAND_UART_TX_GPIO_NUM,COMMAND_UART_RX_GPIO_NUM,-1,-1);
    //step3: install uart1 driver
    if(uart_driver_install(COMMAND_UART_NUM, 2*1024,0,COMMAND_UART_QUEUE_SIZE,&_commnd_uart_event_handle,0)!=ESP_OK){
        return false;
    }
    //step4: set the mode 
    if(uart_set_mode(COMMAND_UART_NUM,UART_MODE_UART)!=ESP_OK){
        return false;
    }
    //register 
    registerUartCMD();
    return true;
}
int 
ESP32Uart::write(const std::string& command)
{
    return uart_write_bytes(COMMAND_UART_NUM,command.c_str(),command.size());
}
int 
ESP32Uart::read(std::string& msg,int size)
{
    uint8_t* buf=new uint8_t[size+1];
    int read_size=uart_read_bytes(COMMAND_UART_NUM,buf,size,0);
    buf[size]='\0';
    msg=std::string((char*)buf);
    delete buf;
    return  read_size;
}