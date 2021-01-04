/****************************************
*   Flash err code  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.19
*****************************************/
#pragma once
namespace AiBox
{
    typedef enum FLashERRCode
    {
        FLASH_ERR_NONE=0,
        FLASH_ERR_NOT_INITIALIZED,
        FLASH_ERR_NOT_FOUND,
        FLASH_ERR_TYPE_MISMATCH,
        FLASH_ERR_READ_ONLY,
        FLASH_ERR_NOT_ENOUGH_SPACE,
        FLASH_ERR_INVALID_NAME,
        FLASH_ERR_INVALID_HANDLE,
        FLASH_ERR_REMOVE_FAILED,
        FLASH_ERR_KEY_TOO_LONG,
        FLASH_ERR_PAGE_FULL,
        FLASH_ERR_INVALID_STATE,
        FLASH_ERR_INVALID_LENGTH,
        FLASH_ERR_NO_FREE_PAGES,
        FLASH_ERR_VALUE_TOO_LONG,
        FLASH_ERR_PART_NOT_FOUND,

        FLASH_ERR_NOT_OPENED=100,
    }flash_err_code_t;
};