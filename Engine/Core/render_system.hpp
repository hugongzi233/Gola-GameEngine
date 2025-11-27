#pragma once

#include "gola_device.hpp"
#include "gola_game_object.hpp"
#include "gola_pipeline.hpp"

// std
#include <memory>
#include <vector>

#include "../UI/gola_imgui.hpp"

namespace gola {
    class GolaCamera;

    class RenderSystem {
    public:
        RenderSystem(
            GolaDevice &device, VkRenderPass renderPass, GolaImgui *imguiPtr);

        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;

        RenderSystem &operator=(const RenderSystem &) = delete;

        void renderGameObjects(
            VkCommandBuffer commandBuffer,
            std::vector<GolaGameObject> &gameObjects,
            const GolaCamera &camera);

        void renderImgui(VkCommandBuffer commandBuffer);

    private:
        void createPipelineLayout();

        void createPipeline(VkRenderPass renderPass);

        GolaDevice &golaDevice;

        std::unique_ptr<GolaPipeline> golaPipeline;
        VkPipelineLayout pipelineLayout;
        GolaImgui *imgui = nullptr;
    };
}
