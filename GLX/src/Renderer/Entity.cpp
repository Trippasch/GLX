#include "Renderer/Entity.h"

Entity::Entity(Model& model) : pModel(&model)
{}

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