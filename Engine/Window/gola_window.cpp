#include "gola_window.hpp"
#include <stdexcept>

namespace gola {
    GolaWindow::GolaWindow(int width, int height, const std::string &title) : width(width), height(height),
                                                                              windowTitle(title) {
        initWindow();
    }

    GolaWindow::~GolaWindow() {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

    void GolaWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
        glfwSetWindowAspectRatio(window, width, height);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        if (!window) {
            throw std::runtime_error("Failed to create GLFW window");
        }
    }

    void GolaWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void GolaWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto app = static_cast<GolaWindow *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
        app->width = width;
        app->height = height;
    }
}
