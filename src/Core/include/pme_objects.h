#pragma once
#include "Core/include/pme_model.h"

// std
#include <memory>
namespace pme
{
    struct Transform2DComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2()
        {
            const float sin_value = glm::sin(rotation);
            const float cos_value = glm::cos(rotation);
            glm::mat2 rotMatrix{{cos_value,sin_value},{-sin_value, cos_value}};

            glm::mat2 scaleMat{{scale.x, .0f}, {0.f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class PmeObject
    {
    public:
        using id_t = unsigned int;

        static PmeObject CreateObject()
        {
            static id_t currentId = 0;
            return PmeObject{currentId++};
        }

    public:
        PmeObject(const PmeObject &) = delete;
        PmeObject &operator=(const PmeObject &) = delete;
        PmeObject(PmeObject &&) = default;
        PmeObject &operator=(PmeObject &&) = default;

    public:
        id_t GetId() { return id; }

        std::shared_ptr<PmeModel> model{};
        glm::vec3 color{};
        Transform2DComponent transform2d;

    private:
        PmeObject(id_t objId) : id{objId} {}

        id_t id;
    };
}