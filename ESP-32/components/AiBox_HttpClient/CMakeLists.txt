# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_HttpClientInterface.cpp"  "AiBox_HttpClientBase.cpp"  "AiBox_ESP32HttpClient.cpp")
set(COMPONENT_REQUIRES  esp_http_client AiBox_DeBug)
register_component()


