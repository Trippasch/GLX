#pragma once

#include "Renderer/Model.h"
#include "Renderer/Transform.h"
#include "Renderer/AABB.h"
#include "Renderer/Sphere.h"

#include <vector>
#include <memory>

class Entity
{
public:
    //Scene graph
    std::vector<std::shared_ptr<Entity>> children;
    Entity* parent = nullptr;

    //Space information
    Transform transform;

    Model* pModel = nullptr;
    std::unique_ptr<AABB> boundingVolume;

    Entity() = default;
    Entity(Model& model);

    AABB getGlobalAABB();

    //Add child. Argument input is argument of any constructor that you create.
    //By default you can use the default constructor and don't put argument input.
    template<typename... TArgs>
    void addChild(TArgs&&... args)
    {
        children.emplace_back(std::make_shared<Entity>(std::forward<TArgs>(args)...));
        children.back()->parent = this;
    }

    //Update transform if it was changed
    void updateSelfAndChild();
    //Force update of transform even if local space don't change
    void forceUpdateSelfAndChild();
    void drawSelfAndChild(GLenum& mode, const Frustum& frustum, Shader& shader, unsigned int& display, unsigned int& total);

    AABB generateAABB(const Model& model);
    Sphere generateSphereBV(const Model& model);
};