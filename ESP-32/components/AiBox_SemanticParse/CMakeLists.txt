# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_SemanticParse.cpp")
set(COMPONENT_REQUIRES json AiBox_DeBug AiBox_System)
register_component()


