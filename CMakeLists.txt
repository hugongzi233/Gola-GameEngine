cmake_minimum_required(VERSION 4.0)
project(GolaGameEngine)

set(CMAKE_CXX_STANDARD 20)

set(VK_SDK_DIR C:/VulkanSDK/1.4.313.2)
set(GLFW_DIR ThirdParty/glfw-3.4)
set(GLM_DIR ThirdParty/glm-1.0.2)

add_executable(GolaGameEngine main.cpp
        Engine/Core/gola_device.cpp
        Engine/Core/gola_model.cpp
        Engine/Core/gola_pipeline.cpp
        Engine/Core/gola_swap_chain.cpp
        Engine/Window/gola_window.cpp
        Engine/gola_app.cpp)

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
        ${GLM_DIR}/glm)
link_directories(${VK_SDK_DIR}/Lib
        ${GLFW_DIR}/lib)
target_link_libraries(GolaGameEngine
        ${GLFW_LIB}
        ${VK_SDK_DIR}/Lib/vulkan-1.lib)

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