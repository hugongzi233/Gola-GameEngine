#include "gola_app.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace gola {
    GolaApp::GolaApp() {
        loadModels();
        createPipelineLayout();

        auto extent = window.getExtent();
        std::cout << extent.width << " " << extent.height << std::endl;
        swapChain = std::make_unique<GolaSwapChain>(device, extent);

        createPipeline();
        // Initialize ImGui after swapchain and pipeline/renderpass are available
        imgui = std::make_unique<GolaImgui>();
        imgui->init(device, *swapChain, window.getGLFWwindow());

        // recreateSwapChain();
        createCommandBuffers();
    }

    GolaApp::~GolaApp() {
        if (pipeline) {
            vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
        }
        if (imgui) {
            imgui->cleanup();
            imgui.reset();
        }
    }

    void GolaApp::run() {
        // 主循环逻辑
        while (!window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
            // Render and update logic would go here
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        vkDeviceWaitIdle(device.device());
    }

    void GolaApp::loadModels() {
        std::vector<GolaModel::Vertex> vertices = {
            // first triangle
            {{-0.9f, -0.9f}, {1.0f, 0.0f, 0.0f}}, // v0
            {{0.9f, -0.9f},  {1.0f, 0.0f, 0.0f}}, // v1
            {{0.9f,  0.9f},  {0.0f, 1.0f, 0.0f}}, // v2
            // second triangle
            {{0.9f,  0.9f},  {0.0f, 1.0f, 0.0f}}, // v2
            {{-0.9f, 0.9f},  {0.0f, 0.0f, 1.0f}}, // v3
            {{-0.9f, -0.9f}, {1.0f, 0.0f, 0.0f}}, // v0
        };
        model = std::make_unique<GolaModel>(device, vertices);
    }

    void GolaApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr; // No descriptor sets for now
        pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants for now
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // No push constants for now
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void GolaApp::createPipeline() {
        auto pipelineConfigInfo = GolaPipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height());
        //PipelineConfigInfo configInfo = GolaPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT);
        pipelineConfigInfo.renderPass = swapChain->getRenderPass();
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<GolaPipeline>(
            device,
            "Engine/shaders/simple_shader.vert.spv",
            "Engine/shaders/simple_shader.frag.spv",
            pipelineConfigInfo);
    }

    void GolaApp::createCommandBuffers() {
        commandBuffers.resize(swapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            recordCommandBuffers(i);
        }
    }

    void GolaApp::recordCommandBuffers(int imageIndex) {
        // Start ImGui frame and build UI for this frame
        if (imgui) {
            imgui->newFrame();
            imgui->buildUI();
        }

        // 重置命令缓冲区
        vkResetCommandBuffer(commandBuffers[imageIndex], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

        pipeline->bind(commandBuffers[imageIndex]);
        model->bind(commandBuffers[imageIndex]);
        model->draw(commandBuffers[imageIndex]);

        // Render ImGui on top
        if (imgui) {
            imgui->render(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }

    void GolaApp::recreateSwapChain() {
        auto extent = window.getExtent();

        // 等待窗口有效大小
        while (extent.width == 0 || extent.height == 0) {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        // 先清理pipeline和command buffer，再清理swapchain
        commandBuffers.clear();
        if (pipeline) {
            pipeline.reset();
        }
        if (swapChain) {
            swapChain.reset();
        }

        swapChain = std::make_unique<GolaSwapChain>(device, extent);
        createPipeline();

        // Re-init ImGui to update image count / render pass
        if (imgui) {
            imgui->cleanup();
            imgui->init(device, *swapChain, window.getGLFWwindow());
        }

        createCommandBuffers();
    }

    void GolaApp::drawFrame() {
        // static int frameCount = 0;
        // frameCount++;
        // std::cout << "=== Frame " << frameCount << " ===" << std::endl;

        if (window.shouldClose()) {
            std::cout << "Window should close!" << std::endl;
            return;
        }

        uint32_t imageIndex;
        VkResult result = swapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // recreateSwapChain();
            // return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image");
        }

        // 等待上一帧的 fence，确保命令缓冲区不在 pending 状态
        // VkFence fence = swapChain->getInFlightFence(imageIndex);
        // vkWaitForFences(device.device(), 1, &fence, VK_TRUE, UINT64_MAX);
        // vkResetFences(device.device(), 1, &fence);

        // Re-record the command buffer for the acquired image so UI and dynamic content are up-to-date
        recordCommandBuffers(imageIndex);

        result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasFramebufferResized()) {
            window.resetFramebufferResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer");
        }
    }
}
