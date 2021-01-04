# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_FlashInterface.cpp"  "AiBox_FlashBase.cpp"  "AiBox_ESPFlash.cpp")
set(COMPONENT_REQUIRES  nvs_flash console AiBox_DeBug)
register_component()


