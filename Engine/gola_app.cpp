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

    std::unique_ptr<GolaModel> createCubeModel(GolaDevice &device, glm::vec3 offset) {
        std::vector<GolaModel::Vertex> vertices = {
            // left face (white)
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            // right face (yellow)
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
            // top face (blue)
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
            // bottom face (green)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            // front face (red)
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            // back face (cyan)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
        };
        for (auto &v: vertices) {
            v.position += offset;
        }

        return std::make_unique<GolaModel>(device, vertices);
    }

    void GolaApp::loadGameObjects() {
        std::shared_ptr<GolaModel> cubeModel = createCubeModel(device, glm::vec3(0.0f, 0.0f, 0.0f));

        for (int i = 0; i < 5; i++) {
            auto gameObject = GolaGameObject::createGameObject();
            gameObject.model = cubeModel;
            gameObject.transform.translation = glm::vec3(0.0f, 0.0f, 0.5f);
            gameObject.transform.scale = glm::vec3(0.5f, 0.5f, 0.5f);
            gameobjects.push_back(std::move(gameObject));
        }
    }

    void GolaApp::initImgui() {
        imgui = std::make_unique<GolaImgui>();
        imgui->init(device, renderer.getSwapChain(), window.getGLFWwindow());
    }
}
