#pragma once

#include "Resources/Model.h"
#include "Resources/ResourceManager.h"
#include "Mesh/Transform.h"
#include "Mesh/Material.h"
#include "Mesh/AABB.h"
#include "Mesh/SphereBV.h"

#include <imgui.h>

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
    bool isGLTF = false;

    // Material information
    Material material;

    //Bounding volume
    std::shared_ptr<AABB> boundingVolume;
    bool drawAABB = false;

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

    virtual void drawSelfAndChild(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) = 0;

    virtual void drawSelfAndChildTranslucent(GLenum& mode, const Frustum& frustum, unsigned int& display, unsigned int& total) = 0;

    virtual void renderGUI(int i) = 0;

    void renderSceneGraphSimple(GLenum mode, Shader shader)
    {
        if (!children.empty()) {
            updateSelfAndChild();
            drawSelfAndChildSimple(mode, shader);
        }
    }

    void renderSceneGraph(GLenum mode, const Frustum& camFrustum, unsigned int& display, unsigned int& total)
    {
        if (!children.empty()) {
            updateSelfAndChild();
            drawSelfAndChild(mode, camFrustum, display, total);
        }
    }

    void renderSceneGraphTranslucent(GLenum mode, const Frustum& camFrustum, unsigned int& display, unsigned int& total)
    {
        if (!children.empty()) {
            updateSelfAndChild();
            drawSelfAndChildTranslucent(mode, camFrustum, display, total);
        }
    }
};