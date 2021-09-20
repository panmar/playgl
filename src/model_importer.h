#pragma once

#include <queue>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "common.h"
#include "model.h"

class AssimpModelImporter {
public:
    ModelData import(const std::filesystem::path& model_path) {
        ModelData model;

        scene = importer.ReadFile(
            model_path.string(),
            aiProcess_Triangulate | aiProcess_LimitBoneWeights |
                aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs);

        if (!scene) {
            throw PlayGlException(
                fmt::format("Assimp error: {}", importer.GetErrorString()));
        }

        if (scene->mRootNode) {
            model.root_transform =
                to_glm_mat44(scene->mRootNode->mTransformation);
        }

        for (u32 i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[i];
            model.model_parts.emplace_back();
            create_model_part(mesh, model.model_parts.back());
        }

        import_model_graph(model.bind_model_graph);
        model.current_model_graph = model.bind_model_graph;

        if (scene->HasAnimations()) {
            if (!import_animations(model.animations,
                                   model.animations_duration)) {
                throw PlayGlException("Failure importing animations");
            }
        }

        model.compute_bounding_sphere();

        print_model_parts(model.model_parts);
        print_model_graph(model.bind_model_graph);
        print_animations(model.animations);

        return model;
    }

private:
    bool create_model_part(aiMesh* mesh, ModelPart& model_part) {
        if (!mesh) {
            assert(false);
            return false;
        }

        std::string mesh_name = mesh->mName.C_Str();
        if (mesh_name.empty()) {
            i32 mesh_index = find_mesh_index(mesh);
            if (mesh_index < 0) {
                return false;
            }
            mesh_name = find_node_name(static_cast<u32>(mesh_index));
        }
        model_part.name = mesh_name;

        bool result = true;
        result &= extract_positions(mesh, model_part.positions);
        extract_normals(mesh, model_part.normals);
        extract_texcoords(mesh, model_part.texcoords, 0);
        result &= extract_indices(mesh, model_part.indices);
        extract_joints_and_weights(mesh, model_part.joints, model_part.weights,
                                   model_part.bones);

        return result;
    }

    void import_model_graph(std::vector<ModelNode>& model_nodes) {
        if (!model_nodes.empty()) {
            return;
        }
        create_model_graph(scene->mRootNode, -1, model_nodes);
    }

    void create_model_graph(aiNode* node, i32 parent,
                            std::vector<ModelNode>& model_nodes) {
        if (!node) {
            return;
        }

        ModelNode model_node{node->mName.C_Str(),
                             to_glm_mat44(node->mTransformation), parent};

        for (u32 i = 0; i < node->mNumMeshes; ++i) {
            u32 mesh_index = node->mMeshes[i];
            model_node.model_parts.push_back(mesh_index);
        }

        model_nodes.push_back(model_node);

        i32 current_node_index = model_nodes.size() - 1;

        if (-1 != parent) {
            model_nodes[parent].children.push_back(current_node_index);
        }

        for (u32 i = 0; i < node->mNumChildren; ++i) {
            create_model_graph(node->mChildren[i], current_node_index,
                               model_nodes);
        }
    }

    i32 find_mesh_index(aiMesh* mesh) {
        for (u32 i = 0; i < scene->mNumMeshes; ++i) {
            if (scene->mMeshes[i] == mesh) {
                return static_cast<i32>(i);
            }
        }
        return -1;
    }

    std::string find_node_name(u32 mesh_index) {
        const string error = "ErrorNoNameFound";

        if (mesh_index >= scene->mNumMeshes) {
            return error;
        }

        std::queue<aiNode*> nodes;
        nodes.push(scene->mRootNode);

        while (!nodes.empty()) {
            aiNode* node = nodes.front();
            nodes.pop();

            for (u32 i = 0; i < node->mNumMeshes; ++i) {
                if (mesh_index == node->mMeshes[i]) {
                    return node->mName.C_Str();
                }
            }

            for (u32 i = 0; i < node->mNumChildren; ++i) {
                aiNode* child = node->mChildren[i];
                nodes.push(child);
            }
        }

        return error;
    }

    bool extract_positions(aiMesh* mesh, std::vector<glm::vec3>& positions) {
        if (!mesh) {
            return false;
        }
        if (mesh->HasPositions()) {
            for (u32 i = 0; i < mesh->mNumVertices; ++i) {
                auto& position = mesh->mVertices[i];
                positions.emplace_back(to_glm_vec3(position));
            }
        }
        return true;
    }

    bool extract_normals(aiMesh* mesh, std::vector<glm::vec3>& normals) {
        if (mesh->HasNormals()) {
            for (u32 i = 0; i < mesh->mNumVertices; ++i) {
                auto& normal = mesh->mNormals[i];
                normals.emplace_back(to_glm_vec3(normal));
            }
            return true;
        }
        return false;
    }

    bool extract_texcoords(aiMesh* mesh, std::vector<glm::vec2>& texcoords,
                           u32 channel) {
        if (mesh->HasTextureCoords(channel)) {
            for (u32 i = 0; i < mesh->mNumVertices; ++i) {
                auto& texcoord = mesh->mTextureCoords[channel][i];
                texcoords.emplace_back(texcoord.x, 1.f - texcoord.y);
            }
            return true;
        }
        return false;
    }

    bool extract_indices(aiMesh* mesh, std::vector<u16>& indices) {
        if (!mesh) {
            return false;
        }
        if (mesh->HasFaces()) {
            for (u32 i = 0; i < mesh->mNumFaces; ++i) {
                auto& face = mesh->mFaces[i];
                if (face.mNumIndices != 3) {
                    printf("Unsupported polygon - %u sided\n",
                           face.mNumIndices);
                    return false;
                }
                for (u32 j = 0; j < face.mNumIndices; ++j) {
                    indices.emplace_back(face.mIndices[j]);
                }
            }
        }
        return true;
    }

    void extract_joints_and_weights(aiMesh* mesh, std::vector<vec4u8>& joints,
                                    std::vector<glm::vec4>& weights,
                                    std::vector<ModelPartBone>& bones) {
        struct VertexInfluence {
            u8 bone_index = 0;
            f32 weight = 0.f;
        };

        std::vector<std::vector<VertexInfluence>> vertices;
        vertices.resize(mesh->mNumVertices);

        for (u32 i = 0; i < mesh->mNumBones; ++i) {
            aiBone* bone = mesh->mBones[i];
            ModelPartBone modelPartBone{bone->mName.C_Str(),
                                        to_glm_mat44(bone->mOffsetMatrix)};
            bones.push_back(modelPartBone);

            for (u32 j = 0; j < bone->mNumWeights; ++j) {
                aiVertexWeight vertex_weight = bone->mWeights[j];
                u32 vertex_index = vertex_weight.mVertexId;
                VertexInfluence vertex_influence;
                vertex_influence.weight = std::min(vertex_weight.mWeight, 1.f);
                vertex_influence.bone_index = i;
                vertices[vertex_index].push_back(vertex_influence);
            }
        }

        joints.resize(mesh->mNumVertices, vec4u8{0, 0, 0, 0});
        weights.resize(mesh->mNumVertices, glm::vec4(0.f, 0.f, 0.f, 0.f));

        for (u32 i = 0; i < mesh->mNumVertices; ++i) {
            for (u32 j = 0; j < vertices[i].size(); ++j) {
                joints[i].v[j] = vertices[i][j].bone_index;
                weights[i][j] = vertices[i][j].weight;
            }
        }
    }

    bool import_animations(ModelData::Animations& animations,
                           ModelData::AnimationsDuration& durations) {
        if (!scene->HasAnimations()) {
            return false;
        }

        for (u32 i = 0; i < scene->mNumAnimations; ++i) {
            aiAnimation* animation = scene->mAnimations[i];
            std::string animation_name = animation->mName.C_Str();
            std::vector<AnimationChannel>& channels =
                animations[animation_name];
            durations[animation_name] = animation->mDuration;

            if (animation_name.empty()) {
                printf("AssimpImporter: Empty animation name.\n");
                return false;
            }

            for (u32 j = 0; j < animation->mNumChannels; ++j) {
                aiNodeAnim* node_anim = animation->mChannels[j];
                std::string channel_name = node_anim->mNodeName.C_Str();
                AnimationChannel channel{channel_name};

                for (u32 p = 0; p < node_anim->mNumPositionKeys; ++p) {
                    aiVectorKey key = node_anim->mPositionKeys[p];
                    channel.keys[(u32)key.mTime].position =
                        to_glm_vec3(key.mValue);
                }

                for (u32 s = 0; s < node_anim->mNumScalingKeys; ++s) {
                    aiVectorKey key = node_anim->mScalingKeys[s];
                    channel.keys[(u32)key.mTime].scale =
                        to_glm_vec3(key.mValue);
                }

                for (u32 r = 0; r < node_anim->mNumRotationKeys; ++r) {
                    aiQuatKey key = node_anim->mRotationKeys[r];
                    channel.keys[(u32)key.mTime].rotation =
                        to_glm_quat(key.mValue);
                }

                channels.push_back(channel);
            }
        }

        return true;
    }

    void print_model_parts(const std::vector<ModelPart>& model_parts) const {
        printf("Model parts:\n");
        for (const auto& part : model_parts) {
            printf("* %s\n", part.name.c_str());
        }
    }

    void print_model_graph_helper(const std::vector<ModelNode>& nodes,
                                  u32 index, u32 depth) const {
        for (u32 i = 0; i < depth; ++i) {
            printf("+");
        }
        printf("%s", nodes[index].name.c_str());
        printf(
            ": [%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, "
            "%.3f, %.3f, %.3f, "
            "%.3f, %.3f, %.3f]\n",
            nodes[index].transform[0][0], nodes[index].transform[0][1],
            nodes[index].transform[0][2], nodes[index].transform[0][3],
            nodes[index].transform[1][0], nodes[index].transform[1][1],
            nodes[index].transform[1][2], nodes[index].transform[1][3],
            nodes[index].transform[2][0], nodes[index].transform[2][1],
            nodes[index].transform[2][2], nodes[index].transform[2][3],
            nodes[index].transform[3][0], nodes[index].transform[3][1],
            nodes[index].transform[3][2], nodes[index].transform[3][3]);
        for (const auto& child : nodes[index].children) {
            print_model_graph_helper(nodes, child, depth + 1);
        }
    }

    void print_animations(const ModelData::Animations& animations) {
        printf("Animations\n");
        for (auto& animation : animations) {
            if (animation.second.empty()) {
                printf("Animation: %s, no keyframes", animation.first.c_str());
            } else {
                printf("Animation: %s, keyframes: %zu\n",
                       animation.first.c_str(),
                       animation.second[0].keys.size());
            }
        }
    }

    void print_model_graph(const std::vector<ModelNode>& nodes) {
        printf("Model nodes:\n");
        print_model_graph_helper(nodes, 0, 0);
    }

    glm::mat4 to_glm_mat44(const aiMatrix4x4t<f32>& m) {
        auto result = glm::make_mat4(&m[0][0]);
        result = glm::transpose(result);
        return result;
    }

    glm::vec3 to_glm_vec3(const aiVector3t<f32>& data) {
        return glm::vec3(data.x, data.y, data.z);
    }

    glm::quat to_glm_quat(const aiQuaterniont<f32>& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    Assimp::Importer importer;
    const aiScene* scene = nullptr;
};