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
    src/Bot_V4.cpp 
    src/fileUtils.cpp 
    src/Game.cpp 
    src/gltools.cpp 
    src/Magics.cpp 
    src/renders.cpp
    src/generator.cpp
    src/evaluation.cpp
)

# Copy models directory to build folder
file(COPY ${PROJECT_SOURCE_DIR}/models DESTINATION ${CMAKE_BINARY_DIR})

# Create the executable
add_executable(MyChessProject ${SOURCES})

# Add include directories
target_include_directories(MyChessProject PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/lib/include
)

# Link directories
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(MyChessProject
    OpenGL::GL
    ${PROJECT_SOURCE_DIR}/lib/freeglut.lib
)

# Copy freeglut.dll to the runtime output directory (e.g., build/Debug or build/Release)
add_custom_command(TARGET MyChessProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/freeglut.dll"
        $<TARGET_FILE_DIR:MyChessProject>
)
