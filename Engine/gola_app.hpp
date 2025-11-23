#pragma once

#include "Window/gola_window.hpp"
#include "Core/gola_pipeline.hpp"
#include "Core/gola_device.hpp"
#include "Core/gola_swap_chain.hpp"
#include "Core/gola_model.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>

namespace gola {
    class GolaApp {
    public:
        static constexpr int WIDTH = 1920;
        static constexpr int HEIGHT = 1080;

        GolaApp();

        ~GolaApp();

        GolaApp(const GolaApp &) = delete;

        GolaApp &operator=(const GolaApp &) = delete;

        void run();

    private:
        void loadModels();

        void createPipelineLayout();

        void createPipeline();

        void createCommandBuffers();

        void drawFrame();

        void recreateSwapChain();

        void recordCommandBuffers(int imageIndex);

        GolaWindow window{WIDTH, HEIGHT, "Gola GameEngine Application"};
        GolaDevice device{window};
        std::unique_ptr<GolaSwapChain> swapChain;
        std::unique_ptr<GolaPipeline> pipeline = nullptr;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<GolaModel> model;
    };
}
