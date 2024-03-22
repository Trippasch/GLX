#pragma once

#include "Resources/Bone.h"
#include "Resources/Model.h"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;

    Animation(const std::string& animationPath, Model* model, int animationIndex)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        if (!scene || !scene->mRootNode) {
            GL_ERROR("Error loading animation file: {0}", importer.GetErrorString());
            throw std::runtime_error("Failed to load animation file");
        }
        // assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[animationIndex];
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();
        m_Bones.reserve(100);
        m_BoneInfoMap.reserve(100);
        ReadHierarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    ~Animation() {}

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
            {
                return Bone.GetBoneName() == name;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }

    inline float GetTicksPerSecond() const { return m_TicksPerSecond; }
    inline float GetDuration() const { return m_Duration;}
    inline const AssimpNodeData& GetRootNode() const { return m_RootNode; }
    inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() const
    { 
        return m_BoneInfoMap;
    }

private:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

    void ReadMissingBones(const aiAnimation* animation, Model& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
        int& boneCount = model.GetBoneCounter(); //getting the m_BoneCounter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data,
                boneInfoMap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
    }

    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.children.reserve(src->mNumChildren);

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
};