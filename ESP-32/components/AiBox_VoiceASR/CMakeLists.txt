# interface cmakelists for partner lib
set(BAIDU_BDS_PATH "$ENV{ADF_PATH}/examples/korvo_du1906/components/bds_light")
set(COMPONENT_ADD_INCLUDEDIRS "inc"  "${BAIDU_BDS_PATH}/include")
set(COMPONENT_SRCS "AiBox_VoiceASR.cpp"  "AiBox_VoiceASRBase.cpp"  "AiBox_VoiceASRBaidu.cpp" "AiBox_author.cpp")
set(COMPONENT_PRIV_REQUIRES json esp-tls AiBox_DeBug AiBox_Console AiBox_System)
set(COMPONENT_EMBED_TXTFILES ./server_root_cert.pem)

register_component()

target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${BAIDU_BDS_PATH}/lib")
if(CONFIG_AUDIO_BOARD_CUSTOM)
target_link_libraries(${COMPONENT_TARGET} INTERFACE bds_light_sdk_cupid)
else()
target_link_libraries(${COMPONENT_TARGET} INTERFACE bds_light_sdk)
endif()



