cmake_minimum_required(VERSION 3.16)
project(pm_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW3 not found.")
endif()

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found.")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Добавляем исполняемый файл
add_executable(${PROJECT_NAME}
    ${SOURCES}
)


# Подключаем include директории
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/Core/include
)

# Линковка библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    Vulkan::Vulkan
)
