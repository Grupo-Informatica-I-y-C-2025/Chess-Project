cmake_minimum_required(VERSION 3.10)

project(MyChessProject)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES
    src/Bitboards.cpp
    src/main.cpp
    src/board.cpp
    src/boardGL.cpp 
    src/Bot_V3.cpp 
    src/Bot_V4.cpp 
    src/fileUtils.cpp 
    src/Game.cpp 
    src/gltools.cpp 
    src/Magics.cpp 
    src/menu.cpp
    src/renders.cpp
)


# Specify include directories for the target after it's created
target_include_directories(MyChessProject PRIVATE ${PROJECT_SOURCE_DIR}/include)
# Directorio donde está el .lib
link_directories(${PROJECT_SOURCE_DIR}/include)

# Vincular la librería (sin la extensión)
target_link_libraries(MyChessProject freeglut)

# Copiar la DLL después del build
add_custom_command(TARGET MyChessProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PROJECT_SOURCE_DIR}/include/freeglut.dll
    $<TARGET_FILE_DIR:MyChessProject>
)

