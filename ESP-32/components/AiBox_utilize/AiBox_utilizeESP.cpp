 /****************************************
*   ESP utilize .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.26
*****************************************/
#include <string>
#include "esp_log.h"
#include "AiBox_utilizeESP.hpp"
const std::string LOG_TAG="ESPUtilize";
#define ARRAY_SIZE_OFFSET   5   //Increase this if print_real_time_stats returns ESP_ERR_INVALID_SIZE
#define STATS_TICKS         pdMS_TO_TICKS(10)
#if configGENERATE_RUN_TIME_STATS==1
void RealTimeCPUStatsTask(TickType_t xTicksToWait)
{
    TaskStatus_t *start_array = NULL, *end_array = NULL;
    UBaseType_t start_array_size, end_array_size;
    uint32_t start_run_time, end_run_time;

    //Allocate array to store current task states
    start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    start_array = (TaskStatus_t*)malloc(sizeof(TaskStatus_t) * start_array_size);
    if (start_array == NULL) {
        ESP_LOGI(LOG_TAG.c_str(), "Malloc the start task status err!!");
        free(start_array);
        return ;
    }
    //Get current task states
    start_array_size = uxTaskGetSystemState(start_array, start_array_size, &start_run_time);
    if (start_array_size == 0) {
        ESP_LOGI(LOG_TAG.c_str(), "Get start task status err!!");
        free(start_array);
        return;
    }

    vTaskDelay(xTicksToWait);

    //Allocate array to store tasks states post delay
    end_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    end_array = (TaskStatus_t*)malloc(sizeof(TaskStatus_t) * end_array_size);
    if (end_array == NULL) {
        ESP_LOGI(LOG_TAG.c_str(), "Malloc the end task status err!!");
        free(start_array);
        return;
    }
    //Get post delay task states
    end_array_size = uxTaskGetSystemState(end_array, end_array_size, &end_run_time);
    if (end_array_size == 0) {
         ESP_LOGI(LOG_TAG.c_str(), "Get end task status err!!");
        free(start_array);
        free(end_array);
        return;
    }

    //Calculate total_elapsed_time in units of run time stats clock period.
    uint32_t total_elapsed_time = (end_run_time - start_run_time);
    if (end_run_time<=start_run_time) {
        ESP_LOGI(LOG_TAG.c_str(), "Total elapsed time is 0!!");
        free(start_array);
        free(end_array);
        return;
    }
    uint32_t test_total_time=0;
    printf("| Task | Core | Run Time | Total Time| Percentage\n");
    //Match each task in start_array to those in the end_array
    for (int i = 0; i < start_array_size; i++) {
        int k = -1;
        for (int j = 0; j < end_array_size; j++) {
            if (start_array[i].xHandle == end_array[j].xHandle){
                k = j;
                //Mark that task have been matched by overwriting their handles
                start_array[i].xHandle = NULL;
                end_array[j].xHandle = NULL;
                break;
            }
        }
        //Check if matching task found
        if (k >= 0) {
            uint32_t task_elapsed_time = end_array[k].ulRunTimeCounter - start_array[i].ulRunTimeCounter;
            uint32_t percentage_time = (uint32_t)(((uint64_t)task_elapsed_time* 100UL) /((uint64_t)total_elapsed_time * portNUM_PROCESSORS));
            ESP_LOGI(LOG_TAG.c_str(),"| %s | %d | %d | %d |%d%%\n", start_array[i].pcTaskName,
                                                                    start_array[i].xCoreID,
                                                                    task_elapsed_time, 
                                                                    total_elapsed_time,
                                                                    percentage_time);
            test_total_time+=task_elapsed_time;
        }
    }
    ESP_LOGI(LOG_TAG.c_str(),"all tast time add:%d",test_total_time);
    //Print unmatched tasks
    for (int i = 0; i < start_array_size; i++) {
        if (start_array[i].xHandle != NULL) {
            ESP_LOGI(LOG_TAG.c_str(),"| %s | Deleted\n", start_array[i].pcTaskName);
        }
    }
    for (int i = 0; i < end_array_size; i++) {
        if (end_array[i].xHandle != NULL) {
           ESP_LOGI(LOG_TAG.c_str(),"| %s | Created\n", end_array[i].pcTaskName);
        }
    }
    free(start_array);
    free(end_array);
    return ;
}
static void stats_task(void *arg)
{
    //Print real time stats periodically
    while (1) {
        RealTimeCPUStatsTask(STATS_TICKS);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void printRealTimeCPUStats(){
    xTaskCreatePinnedToCore(stats_task, "CPU_Task", 4096, NULL, 20, NULL, tskNO_AFFINITY);
}
#endif