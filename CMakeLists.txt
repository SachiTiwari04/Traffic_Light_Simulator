cmake_minimum_required(VERSION 3.10)

# Project name and version
project(TrafficLightSimulator VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Windows
if (WIN32)
    # Link against OpenGL and GLUT
    find_package(OpenGL REQUIRED)
    
    # Add the executable
    add_executable(${PROJECT_NAME} main.cpp)
    
    # Link OpenGL and other required libraries
    target_link_libraries(${PROJECT_NAME} 
        OpenGL::GL 
        glu32 
        freeglut
    )
    
    # Copy required DLLs to the output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FREEGLUT_DLL_PATH}/freeglut.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying FreeGLUT DLL to output directory"
    )
    
else()
    # For Linux/macOS
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    
    # Add the executable
    add_executable(${PROJECT_NAME} main.cpp)
    
    # Link OpenGL and GLUT
    target_link_libraries(${PROJECT_NAME} 
        OpenGL::GL 
        GLUT::GLUT
    )
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
