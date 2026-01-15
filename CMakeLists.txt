# 设置对cmake的最小版本
cmake_minimum_required(VERSION 3.29.1)

# 设置工程名
project(monster_war)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# 添加编译器选项（仅在 Debug 模式下开启严格警告）
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# 需要的库
find_package(SFML REQUIRED COMPONENTS Audio Graphics)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# 设置目标对象（可执行文件）的输出目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# 获取指定目录下的所有cpp文件，用SRC_LIST变量代替
# 如果src目录下的cpp文件很多，在生成可执行文件时一个个写出来十分不便，所以可以使用这个命令方便使用
file(GLOB_RECURSE SRC_LIST CMAKE_CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/thirdparty/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/src/thirdparty/imgui_sfml/*.cpp
)

# 生成可执行文件，这里的所有cpp文件用变量替代
add_executable(${PROJECT_NAME} ${SRC_LIST})

# 指定需要的头文件目录
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/thirdparty
        ${PROJECT_SOURCE_DIR}/include/thirdparty/imgui
        ${PROJECT_SOURCE_DIR}/include/thirdparty/imgui_sfml
        ${PROJECT_SOURCE_DIR}/include/thirdparty/entt
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SFML::System
        SFML::Window
        SFML::Audio
        SFML::Graphics
        OpenGL::GL
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)