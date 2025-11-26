#include "gola_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

#include <chrono>
#include <iostream>
#include <thread>

#include "Core/render_system.hpp"

namespace gola {
    GolaApp::GolaApp() {
        loadGameObjects();
    }

    GolaApp::~GolaApp() {
    }

    void GolaApp::run() {
        initImgui();
        RenderSystem renderSystem(device, renderer.getSwapChainRenderPass(), imgui.get());
        // 主循环逻辑
        while (!window.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameobjects);
                renderSystem.renderImgui(commandBuffer);

                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    void GolaApp::loadGameObjects() {
        std::vector<GolaModel::Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // v0
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // v1
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // v2
        };
        auto model = std::make_shared<GolaModel>(device, vertices);

        auto triangle = GolaGameObject::createGameObject();
        triangle.model = model;
        triangle.color = {0.1f, 0.1f, 0.1f};
        triangle.transform2d.translation.x = 0.f;
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameobjects.push_back(std::move(triangle));
    }

    void GolaApp::initImgui() {
        imgui = std::make_unique<GolaImgui>();
        imgui->init(device, renderer.getSwapChain(), window.getGLFWwindow());
    }
}
