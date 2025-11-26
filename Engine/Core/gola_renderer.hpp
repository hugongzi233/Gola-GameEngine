#pragma once

#include "gola_device.hpp"
#include "gola_swap_chain.hpp"
#include "../Window/gola_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace gola {
    class GolaRenderer {
    public:
        GolaRenderer(GolaWindow &window, GolaDevice &device);

        ~GolaRenderer();

        GolaRenderer(const GolaRenderer &) = delete;

        GolaRenderer &operator=(const GolaRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return golaSwapChain->getRenderPass(); }
        GolaSwapChain &getSwapChain() { return *golaSwapChain; }
        float getAspectRatio() const { return golaSwapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();

        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();

        void freeCommandBuffers();

        void recreateSwapChain();

        GolaWindow &golaWindow;
        GolaDevice &golaDevice;
        std::unique_ptr<GolaSwapChain> golaSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
} // namespace gola
