# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_BoardInterface.cpp"  "AiBox_BoardBase.cpp"  "AiBox_BoardESP.cpp")
set(COMPONENT_REQUIRES  console nvs_flash esp_peripherals input_key_service AiBox_DeBug AiBox_System)
register_component()


