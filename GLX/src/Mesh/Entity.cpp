#include "Mesh/Entity.h"

Entity::Entity()
{
    ResourceManager::LoadShader("assets/shaders/gizmoVS.glsl", "assets/shaders/gizmoFS.glsl", nullptr, "gizmo");
    ResourceManager::GetShader("gizmo").Use().SetVector3f("color", glm::vec3(1.0f, 1.0f, 1.0f));

    ResourceManager::GetShader("gizmo").Use().SetBlockIndex("Matrices", 0);
}

AABB Entity::getGlobalAABB()
{
    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(boundingVolume->center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = transform.getRight() * boundingVolume->extents.x;
    const glm::vec3 up = transform.getUp() * boundingVolume->extents.y;
    const glm::vec3 forward = transform.getForward() * boundingVolume->extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    return AABB(globalCenter, newIi, newIj, newIk);
}

void Entity::updateSelfAndChild()
{
    if (transform.isDirty()) {
        forceUpdateSelfAndChild();
        return;
    }

    for (auto&& child : children) {
        child->updateSelfAndChild();
    }
}

void Entity::forceUpdateSelfAndChild()
{
    if (parent)
        transform.computeModelMatrix(parent->transform.getModelMatrix());
    else
        transform.computeModelMatrix();

    for (auto&& child : children) {
        child->forceUpdateSelfAndChild();
    }
}