# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_WiFiInterface.cpp"  "AiBox_WiFiBase.cpp"  "AiBox_WiFiESP32.cpp")
set(COMPONENT_REQUIRES  console nvs_flash AiBox_Flash AiBox_DeBug AiBox_System)
register_component()


