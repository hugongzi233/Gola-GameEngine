#pragma once
#include "gola_model.hpp"
#include "vec3.hpp"

namespace gola {
    struct Transform2D {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMat{{c, s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotMat * scaleMat;
        }
    };

    class GolaGameObject {
    public:
        using id_t = unsigned int;

        static GolaGameObject createGameObject() {
            static id_t currentId = 0;
            return GolaGameObject{currentId++};
        }

        GolaGameObject(const GolaGameObject &) = delete;

        GolaGameObject &operator=(const GolaGameObject &) = delete;

        GolaGameObject(GolaGameObject &&) = default;

        GolaGameObject &operator=(GolaGameObject &&) = default;

        id_t getId() const {
            return id;
        }

        const id_t id;
        std::shared_ptr<GolaModel> model;

        glm::vec3 color;
        Transform2D transform2d;

    private:
        GolaGameObject(id_t objId) : id{objId} {
        }
    };
}
