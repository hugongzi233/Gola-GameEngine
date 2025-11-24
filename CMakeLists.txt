cmake_minimum_required(VERSION 3.31)
project(GolaGameEngine)

set(CMAKE_CXX_STANDARD 26)
#set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(VK_SDK_DIR C:/VulkanSDK/1.4.313.2)
set(GLFW_DIR ThirdParty/glfw-3.4)
set(GLM_DIR ThirdParty/glm-1.0.2)
set(VK_BOOTSTRAP_DIR ThirdParty/vk-bootstrap)

add_executable(GolaGameEngine main.cpp
        Engine/Core/gola_device.cpp
        Engine/Core/gola_model.cpp
        Engine/Core/gola_pipeline.cpp
        Engine/Core/gola_swap_chain.cpp
        Engine/Window/gola_window.cpp
        Engine/gola_app.cpp
        Engine/Core/gola_renderer.cpp)

find_library(GLFW_LIB
        NAMES glfw3dll glfw3
        PATHS "${GLFW_DIR}/lib"
        NO_DEFAULT_PATH
)

if (NOT GLFW_LIB)
    message(FATAL_ERROR "GLFW library not found in ${GLFW_DIR}/lib")
endif()

include_directories(${VK_SDK_DIR}/Include
        ${GLFW_DIR}/include
        ${GLM_DIR}/glm
        ${VK_BOOTSTRAP_DIR}/src)
link_directories(${VK_SDK_DIR}/Lib
        ${GLFW_DIR}/lib
        ${VK_BOOTSTRAP_DIR}/build)
target_link_libraries(GolaGameEngine
        ${GLFW_LIB}
        ${VK_SDK_DIR}/Lib/vulkan-1.lib
        vk-bootstrap::vk-bootstrap)

add_subdirectory(ThirdParty/vk-bootstrap)

if(EXISTS "${CMAKE_SOURCE_DIR}/${GLFW_DIR}/lib/glfw3.dll")
    message(STATUS "GLFW DLL found: ${CMAKE_SOURCE_DIR}/${GLFW_DIR}/lib/glfw3.dll")
    add_custom_command(TARGET GolaGameEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/${GLFW_DIR}/lib/glfw3.dll"
            $<TARGET_FILE_DIR:GolaGameEngine>
            COMMENT "Copying GLFW3 DLL to output directory"
    )
else()
    message(WARNING "GLFW DLL not found at: ${CMAKE_SOURCE_DIR}/${GLFW_DIR}/lib/glfw3.dll")
endif()