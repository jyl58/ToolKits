# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_PRIV_INCLUDEDIRS "service")

set(COMPONENT_SRCS "AiBox_BleInterface.cpp"  "AiBox_BleBase.cpp"  "AiBox_BleESP.cpp"  "service/BleServiceBase.cpp" "service/BleServiceWiFiConfigTable.cpp")
set(COMPONENT_REQUIRES esp_peripherals bluetooth_service AiBox_DeBug AiBox_System AiBox_Board)
set(COMPONENT_PRIV_REQUIRES bt)
register_component()


