/****************************************
*   ESP Board  data .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.18
*****************************************/
#pragma once
#include <string>
namespace AiBox
{
    typedef enum ButtonID
    {
        BUTTON_ID_VOLDOWN=120,
        BUTTON_ID_VOLUP,
        BUTTON_ID_PLAY,
        BUTTON_ID_MUTE
    }button_id_t;
    typedef enum ButtonType
    {
        BUTTON_TYPE_DOWN=1,  // short time
        BUTTON_TYPE_UP=2,    //short time
        BUTTON_TYPE_PRESS=3,  //long time 
        BUTTON_TYPE_PRESS_UP=4 //long time
    }button_type_t;
    typedef struct ButtonData
    {
        button_id_t _button_id;
        button_type_t _type;

    }button_data_t;
};