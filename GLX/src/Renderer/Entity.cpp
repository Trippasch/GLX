#include "Renderer/Entity.h"

Entity::Entity(Model& model) : pModel(&model)
{
    boundingVolume = std::make_unique<AABB>(generateAABB(model));
    // boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
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

void Entity::drawSelfAndChild(GLenum& mode, const Frustum& frustum, Shader& shader, unsigned int& display, unsigned int& total)
{
    if (boundingVolume->isOnFrustum(frustum, transform))
    {
        shader.SetMatrix4(1, transform.getModelMatrix());
        pModel->Draw(mode, shader);
        display++;
    }
    total++;

    for (auto&& child : children)
    {
        child->drawSelfAndChild(mode, frustum, shader, display, total);
    }
}

AABB Entity::generateAABB(const Model& model)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
    for (auto&& mesh : model.meshes)
    {
        for (auto&& vertex : mesh.vertices)
        {
            minAABB.x = std::min(minAABB.x, vertex.Position.x);
            minAABB.y = std::min(minAABB.y, vertex.Position.y);
            minAABB.z = std::min(minAABB.z, vertex.Position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
        }
    }
    return AABB(minAABB, maxAABB);
}

Sphere Entity::generateSphereBV(const Model& model)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
    for (auto&& mesh : model.meshes)
    {
        for (auto&& vertex : mesh.vertices)
        {
            minAABB.x = std::min(minAABB.x, vertex.Position.x);
            minAABB.y = std::min(minAABB.y, vertex.Position.y);
            minAABB.z = std::min(minAABB.z, vertex.Position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
        }
    }

    return Sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}