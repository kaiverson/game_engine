cmake_minimum_required(VERSION 3.15)
project(game_engine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories for binaries and build files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Add the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/Users/user/Desktop/Code/C/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Find the libraries using vcpkg
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add the source files
file(GLOB SRC_FILES 
    "src/*.cpp"
    "src/*.c"
)

# Add the executable
add_executable(game_engine ${SRC_FILES})

# Link the libraries
target_link_libraries(game_engine PRIVATE 
    glfw
    assimp::assimp
    imgui::imgui
)

# Link the GLFW and Assimp libraries
# target_link_libraries(game_engine ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a)
# target_link_libraries(game_engine assimp::assimp)

# Copy the GLFW DLL to the output directory
add_custom_command(TARGET game_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/glfw3.dll
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)