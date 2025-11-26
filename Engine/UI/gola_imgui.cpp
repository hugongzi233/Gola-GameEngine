//
// Created by dream on 2025/11/25.
//

#include "gola_imgui.hpp"

// Implementation-only includes
#include <ostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <stdexcept>
#include <vector>

namespace gola {
    static VkDescriptorPool createImguiDescriptorPool(VkDevice device) {
        // Create a descriptor pool following the ImGui example recommended sizes
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = 1;
        pool_info.pPoolSizes = pool_sizes;

        VkDescriptorPool descriptorPool;
        if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create ImGui descriptor pool");
        }
        return descriptorPool;
    }

    GolaImgui::~GolaImgui() {
        // Ensure cleanup if user forgot
    }

    void GolaImgui::createDescriptorPool(VkDevice device) {
        imguiDescriptorPool = createImguiDescriptorPool(device);
    }

    void GolaImgui::init(GolaDevice &device, GolaSwapChain &swapChain, GLFWwindow *window) {
        // Store device for cleanup
        device_ = device.device();
        createDescriptorPool(device_);

        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //ImGui::StyleColorsDark();
        setupCustomFont();
        setupCustomStyle();

        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device.getInstance();
        init_info.PhysicalDevice = device.getPhysicalDevice();
        init_info.Device = device_;
        init_info.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily;
        init_info.Queue = device.graphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = imguiDescriptorPool;
        init_info.DescriptorPoolSize = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = static_cast<uint32_t>(swapChain.imageCount());
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;

        // Fill pipeline info inside init_info
        init_info.PipelineInfoMain.RenderPass = swapChain.getRenderPass();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info);

        // Upload Fonts - current backend creates fonts automatically on NewFrame but call manually to be safe
        ImGui_ImplVulkan_SetMinImageCount(static_cast<uint32_t>(swapChain.imageCount()));

        std::println("Imgui initialized");
    }

    void GolaImgui::newFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GolaImgui::setupCustomFont() {
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr,
                                     io.Fonts->GetGlyphRangesChineseFull());
    }

    void GolaImgui::setupCustomStyle() {
        ImGuiStyle &style = ImGui::GetStyle();

        // 圆角
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.PopupRounding = 5.0f;

        // 颜色
        ImVec4 *colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    }

    void GolaImgui::buildUI() {
        // 1. Debug window
        ImGui::Begin("Debug Info");
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Triangles: %d", triangleCount);
        ImGui::Text("Draw Calls: %d", drawCallCount);
        ImGui::End();

        // 2. Controls panel
        ImGui::Begin("Controls");
        ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
        ImGui::ColorEdit3("Clear Color", clearColor);
        ImGui::Checkbox("VSync", &vsyncEnabled);
        ImGui::End();

        // 3. Performance window
        ImGui::Begin("Performance", &showPerformanceWindow);
        ImGui::Text("Performance graphs would go here测试");
        ImGui::End();
    }

    void GolaImgui::render(VkCommandBuffer commandBuffer) {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer, VK_NULL_HANDLE);
    }

    void GolaImgui::cleanup() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (imguiDescriptorPool != VK_NULL_HANDLE && device_ != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(device_, imguiDescriptorPool, nullptr);
            imguiDescriptorPool = VK_NULL_HANDLE;
        }
    }
} // namespace gola
