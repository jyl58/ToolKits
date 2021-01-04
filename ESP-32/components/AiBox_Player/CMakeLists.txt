# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_PlayerInterface.cpp"  "AiBox_BasePlayer.cpp" "AiBox_ESPPlayer.cpp")
set(COMPONENT_REQUIRES   audio_sal esp-adf-libs audio_pipeline audio_stream audio_hal audio_board esp_peripherals bluetooth_service AiBox_DeBug AiBox_Console AiBox_System AiBox_Board AiBox_Bluetooth AiBox_PlayManage)
register_component()


