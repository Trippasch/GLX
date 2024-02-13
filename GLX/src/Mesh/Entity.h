#pragma once

#include "Resources/Model.h"
#include "Resources/ResourceManager.h"
#include "Mesh/Transform.h"
#include "Mesh/Material.h"
#include "Mesh/AABB.h"
#include "Mesh/SphereBV.h"

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

    // Material information
    Material material;

    //Bounding volume
    std::shared_ptr<AABB> boundingVolume;

    virtual ~Entity() = default;

    AABB getGlobalAABB();

    //Add child. Argument input is argument of any constructor that you create.
    //By default you can use the default constructor and don't put argument input.
    template<typename T, typename... TArgs>
    void addChild(TArgs&&... args)
    {
        static_assert(std::is_base_of<Entity, T>::value, "T must be a subclass of Entity");
        children.emplace_back(std::make_shared<T>(std::forward<TArgs>(args)...));
        children.back()->parent = this;
    }

    //Update transform if it was changed
    void updateSelfAndChild();
    //Force update of transform even if local space don't change
    void forceUpdateSelfAndChild();

    virtual void drawSelfAndChildSimple(GLenum& mode, Shader& shader) = 0;

    virtual void drawSelfAndChild(GLenum& mode, const Frustum& frustum, Shader& shader, unsigned int& display, unsigned int& total) = 0;

    void renderSceneGraphSimple(GLenum mode, Shader shader)
    {
        drawSelfAndChildSimple(mode, shader);
        updateSelfAndChild();
    }

    void renderSceneGraph(GLenum mode, Shader shader, const Frustum& camFrustum, unsigned int& display, unsigned int& total)
    {
        drawSelfAndChild(mode, camFrustum, shader, display, total);
        updateSelfAndChild();
    }
};