# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_PlayManage.cpp")
set(COMPONENT_REQUIRES  AiBox_DeBug AiBox_Console AiBox_System)	

register_component()
