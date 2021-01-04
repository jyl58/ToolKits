# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_SystemInterface.cpp"  "AiBox_SystemBase.cpp"  "AiBox_ESPSystem.cpp" "AiBox_SystemStatusData.cpp")
set(COMPONENT_REQUIRES lwip AiBox_DeBug AiBox_Flash AiBox_Console AiBox_WiFi AiBox_Bluetooth AiBox_Player AiBox_ProcessASR)
register_component()


