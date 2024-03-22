#pragma once

#include <vector>
#include <list>

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Utils/assimp_glm_helpers.h"

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone
{
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        : m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    void Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }

    const glm::mat4& GetLocalTransform() const
    {
        return m_LocalTransform;
    }

    const std::string& GetBoneName() const
    {
        return m_Name;
    }

    int GetBoneID() const
    {
        return m_ID;
    }

    int GetPositionIndex(float animationTime) const
    {
        for (int index = 0; index < m_NumPositions - 1; ++index) {
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        }
        return -1; // return -1 if no index found
    }

    int GetRotationIndex(float animationTime) const
    {
        for (int index = 0; index < m_NumRotations - 1; ++index) {
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        }
        return -1; // return -1 if no index found
    }

    int GetScaleIndex(float animationTime) const
    {
        for (int index = 0; index < m_NumScalings - 1; ++index) {
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        }
        return -1; // return -1 if no index found
    }

private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

        int p0Index = GetPositionIndex(animationTime);
        if (p0Index == -1) {
            GL_ERROR("Error: p0Index == -1");
            return glm::mat4(1.0f);
        }
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
            m_Positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
            , scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
        {
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        if (p0Index == -1) {
            GL_ERROR("Error: p0Index == -1");
            return glm::mat4(1.0f);
        }
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
            m_Rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
            , scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalings)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        if (p0Index == -1) {
            GL_ERROR("Error: p0Index == -1");
            return glm::mat4(1.0f);
        }
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
            m_Scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
            , scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
};