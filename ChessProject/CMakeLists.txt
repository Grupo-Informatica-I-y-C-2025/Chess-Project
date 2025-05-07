cmake_minimum_required(VERSION 3.10)
project(MyChessProject LANGUAGES CXX)

# Usa C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opcional: ejecutables en build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Lista de fuentes
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

# Copy models directory to build folder
file(COPY ${PROJECT_SOURCE_DIR}/models DESTINATION ${CMAKE_BINARY_DIR})

# Crea el ejecutable
add_executable(MyChessProject ${SOURCES})

# 1) Incluir headers
target_include_directories(MyChessProject
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# 2) Indicar al linker dónde buscar .lib
target_link_directories(MyChessProject
    PRIVATE
        ${PROJECT_SOURCE_DIR}/lib
)

# 3) Linkea contra freeglut.lib (sin la extensión)
target_link_libraries(MyChessProject
    PRIVATE
        freeglut
)

# 4) Copia la DLL al directorio del ejecutable tras compilar
add_custom_command(TARGET MyChessProject
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/lib/freeglut.dll
            $<TARGET_FILE_DIR:MyChessProject>
    COMMENT "Copiando freeglut.dll al directorio del ejecutable"
)
