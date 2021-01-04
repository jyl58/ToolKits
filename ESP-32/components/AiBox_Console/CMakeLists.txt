# interface cmakelists for partner lib
set(COMPONENT_ADD_INCLUDEDIRS "inc")
set(COMPONENT_SRCS "AiBox_ESPConsole.cpp"  "AiBox_ConsoleBase.cpp"  "AiBox_Console.cpp")
set(COMPONENT_REQUIRES fatfs console AiBox_DeBug AiBox_Flash AiBox_System)
register_component()


