/****************************************
*   ESP console .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#include "AiBox_ESPConsole.hpp"
#include "AiBox_ESPFlash.hpp"
#include "AiBox_ESPSystem.hpp"
#include "AiBox_ProcessASR.hpp"
#include "AiBox_SemanticParse.hpp"
#include "AiBox_PlayManage.hpp"
using namespace AiBox;
const std::string LOG_TAG="ESPConsole";
static void registerASRCMD()
{
    esp_console_cmd_t join_cmd;
    join_cmd.command = "asr";
    join_cmd.help = "process ASR result";
    join_cmd.hint = NULL;
    join_cmd.func = &processASRCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
static void registerSemanticCMD()
{
    esp_console_cmd_t join_cmd;
    join_cmd.command = "semantic";
    join_cmd.help = "process Semantic result";
    join_cmd.hint = NULL;
    join_cmd.func = &processSemanticCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
static void 
registerPlayManageCMD()
{
    esp_console_cmd_t join_cmd;
    join_cmd.command = "playManage";
    join_cmd.help = "play Manage parm";
    join_cmd.hint = NULL;
    join_cmd.func = &playManageCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
bool 
ESPConsole::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Init console");
    /* Disable buffering on stdin*/
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    uart_config_t uart_config;
    memset(&uart_config,0,sizeof(uart_config_t));
    uart_config.baud_rate = CONFIG_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.use_ref_tick = true;
 
    ESP_ERROR_CHECK(uart_param_config((uart_port_t)CONFIG_CONSOLE_UART_NUM, &uart_config));

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK(uart_driver_install((uart_port_t)CONFIG_CONSOLE_UART_NUM,256, 0, 0, NULL, 0));

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver((uart_port_t)CONFIG_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config;
    memset(&console_config,0,sizeof(esp_console_config_t));
    console_config.max_cmdline_args = 8;
    console_config.max_cmdline_length = 256;
#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif

    ESP_ERROR_CHECK(esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(20);

    registerDefaultCMD();
    return true;
}
void 
ESPConsole::registerDefaultCMD()
{
    //registe the flash cmd
    registerFlashCMD();
    //registe the sustem cmd
    registerSystemCMD();
    //register ASR CMD;
    registerASRCMD();
    //register Semantic CMD;
    registerSemanticCMD();
    //register playManage CMD
    registerPlayManageCMD();
}
void 
ESPConsole::exec()
{
    while(1){
         /* Get a line using linenoise.
         * The line is returned when ENTER is pressed.
         */
        char* line = linenoise(_prompt.c_str());
        if (line == NULL) { /* Ignore empty lines */
            continue;
        }
        /* Add the command to the history */
        linenoiseHistoryAdd(line);
        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGI(LOG_TAG.c_str(),"Unrecognized command");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            ESP_LOGI(LOG_TAG.c_str(),"Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(err));
        } else if (err != ESP_OK) {
            ESP_LOGI(LOG_TAG.c_str(),"Internal error: %s\n", esp_err_to_name(err));
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
        //sleep 500ms
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}