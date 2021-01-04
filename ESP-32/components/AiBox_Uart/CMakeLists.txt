# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_UART.cpp"  "AiBox_UARTBase.cpp"  "AiBox_UARTESP32.cpp")
set(COMPONENT_REQUIRES  driver console AiBox_DeBug)
register_component()


