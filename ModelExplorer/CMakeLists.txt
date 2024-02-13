cmake_minimum_required(VERSION 3.8)

add_executable(ModelExplorer "main.cpp" "Core/AApplication.h" "Core/AWindow.h" "Core/AWindow.cpp" "Core/AApplication.cpp" "ModelExplorer.h" "ModelExplorer.cpp" "ExplorerWindow.h" "ExplorerWindow.cpp" "Core/Renderer/Renderer.h" "Core/Renderer/Renderer.cpp" "Core/Provider/VulkanApplicationInfoProvider.h" "Core/Provider/VulkanApplicationInfoProvider.cpp" "Core/Provider/IProvider.h")
target_include_directories(ModelExplorer PUBLIC "../glfw-3.3.9/include"
                                                "C:/VulkanSDK/1.3.275.0/Include"
                                                "./")
target_link_directories(ModelExplorer PUBLIC "../glfw-3.3.9/lib-vc2022"
                                             "C:/VulkanSDK/1.3.275.0/Lib")
target_link_libraries(ModelExplorer PUBLIC "glfw3.lib"
                                           "vulkan-1.lib")