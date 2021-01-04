/****************************************
*   ESP32 Log  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.25
*****************************************/
#pragma once
#include "esp_log.h"
#define LOGI( tag, format, ... ) ESP_LOGI( tag, format, ##__VA_ARGS__)
#define LOGD( tag, format, ... ) ESP_LOGD( tag, format, ##__VA_ARGS__)
//memory malloc
#include <cstring>
#include "esp_heap_caps.h"
inline void* sysMemMalloc(size_t size){
#ifdef  CONFIG_SPIRAM_SUPPORT
    return heap_caps_malloc( size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    return heap_caps_malloc( size, MALLOC_CAP_INTERNAL| MALLOC_CAP_8BIT);
#endif
}
inline void sysMemFree(void *ptr)
{
    heap_caps_free(ptr);
    ptr=NULL;
}
inline void* sysMemMallocInner(size_t n, size_t size)
{
    void *data =  NULL;
    data = heap_caps_malloc(n * size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (data) {
        memset(data, 0, n * size);
    }
    return data;
}
