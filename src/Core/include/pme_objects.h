#pragma once
#include "Core/include/pme_model.h"

// std
#include <memory>
namespace pme
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation;

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
        
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
        TransformComponent transform;

    private:
        PmeObject(id_t objId) : id{objId} {}

        id_t id;
    };
}