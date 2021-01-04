# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_ProcessASR.cpp"  )
set(COMPONENT_REQUIRES  json AiBox_DeBug AiBox_Console AiBox_System AiBox_HttpClient AiBox_SemanticParse)
register_component()


