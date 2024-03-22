#pragma once

#include "Resources/Animation.h"
#include "Resources/Bone.h"

class Animator
{
public:
    Animator() : m_CurrentTime(0.0f), m_DeltaTime(0.0f)
    {
        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void AddAnimation(const std::string& name, Animation* animation)
    {
        m_Animations[name] = animation;
    }

    void SetAnimation(const std::string& name)
    {
        if (m_Animations.find(name) != m_Animations.end())
            m_CurrentAnimationName = name;
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_Animations[m_CurrentAnimationName]) {
            m_CurrentTime += m_Animations[m_CurrentAnimationName]->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_Animations[m_CurrentAnimationName]->GetDuration());
            CalculateBoneTransform(&m_Animations[m_CurrentAnimationName]->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void PlayAnimation(const std::string& name)
    {
        if (m_Animations.find(name) != m_Animations.end()) {
            m_CurrentAnimationName = name;
            m_CurrentTime = 0.0f;
        }
    }

    void CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform)
    {
        const std::string& nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_Animations[m_CurrentAnimationName]->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        const auto& boneInfoMap = m_Animations[m_CurrentAnimationName]->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap.at(nodeName).id;
            glm::mat4 offset = boneInfoMap.at(nodeName).offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->children.size(); i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    const std::vector<glm::mat4>& GetFinalBoneMatrices() const
    {
        return m_FinalBoneMatrices;
    }

private:
    std::unordered_map<std::string, Animation*> m_Animations;
    std::string m_CurrentAnimationName;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    float m_CurrentTime;
    float m_DeltaTime;
};