cmake_minimum_required(VERSION 3.10)
project(TaskManagerCLI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for SQLiteCpp
include_directories(${CMAKE_SOURCE_DIR}/external/SQLiteCpp/include)

# Add SQLiteCpp as a subdirectory
add_subdirectory(external/SQLiteCpp)

# Add executable
add_executable(task_manager task_manager.cpp)

# Link libraries
target_link_libraries(task_manager PRIVATE SQLiteCpp pthread dl)