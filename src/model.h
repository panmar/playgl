#pragma once

#include "common.h"

class BoundingSphere {
public:
    static BoundingSphere from_points(const std::vector<glm::vec3>& data) {
        glm::vec3 min{FLT_MAX, FLT_MAX, FLT_MAX};
        glm::vec3 max{-FLT_MAX, -FLT_MAX, -FLT_MAX};
        for (auto& d : data) {
            min = {std::min(min.x, d.x), std::min(min.y, d.y),
                   std::min(min.z, d.z)};
            max = {std::max(max.x, d.x), std::max(max.y, d.y),
                   std::max(max.z, d.z)};
        }
        auto center = (min + max) / 2.f;
        auto radius = glm::distance(min, max) / 2.f;
        return BoundingSphere{center, radius};
    }

    void combine(const BoundingSphere& other) {
        auto dist = glm::distance(center, other.center);
        if (dist + radius < other.radius) {
            if (other.radius > radius) {
                center = other.center;
                radius = other.radius;
            }
        } else {
            auto new_radius = (radius + other.radius + dist) / 2.f;
            auto new_center =
                center + (other.center - center) * (new_radius - radius) / dist;
            radius = new_radius;
            center = new_center;
        }
    }

    glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
    f32 radius = 0;
};

struct ModelNode {
    std::string name;
    glm::mat4 transform;
    i32 parent;
    std::vector<u32> children;
    std::vector<u32> model_parts;
};

// NOTE (panmar): The representation for a bone node directly influencing
// ModelPart. There can be other bone nodes not directly influencing ModelPart
// (e.g. controllers or IK bones). Those bones influence transformation of
// ModelPartBones and are usually affected by animation.

struct ModelPartBone {
    std::string name;
    glm::mat4 part_to_bone_transform;
};

struct ModelPart {
    std::string name;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<u16> indices;
    std::vector<vec4u8> joints;
    std::vector<glm::vec4> weights;
    std::vector<ModelPartBone> bones;
    BoundingSphere bounding_sphere;
};

struct AnimationTransform {
    glm::mat4 get_transform() const {
        auto identity = glm::mat4(1.f);
        auto translate = glm::translate(identity, position);
        auto rotation_mat4 = glm::toMat4(rotation);
        auto scale_mat4 = glm::scale(identity, scale);
        return translate * rotation_mat4 * scale_mat4;
    }
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

struct AnimationChannel {
    std::string name;
    std::map<u32, AnimationTransform> keys;
};

struct ModelData {
    glm::mat4 get_absolute_node_transform(const std::string& node_name) const {
        for (u32 i = 0; i < current_model_graph.size(); ++i) {
            const ModelNode& node = current_model_graph[i];
            if (node_name == node.name) {
                i32 index = i;
                auto transform = glm::mat4(1.f);
                while (index != -1) {
                    // TODO(panmar): Is this order corrent for OpenGL?
                    transform =
                        current_model_graph[index].transform * transform;
                    index = current_model_graph[index].parent;
                }
                return transform;
            }
        }
        printf("Model::get_absolute_node_transform(): no node named %s\n",
               node_name.c_str());
        assert(false);
        return glm::mat4(1.f);
    }

    std::vector<glm::mat4> get_skinning_palette(
        const std::vector<ModelPartBone>& bones) const {
        std::vector<glm::mat4> pallete;
        auto inv_root_transform = glm::inverse(root_transform);
        for (const auto& bone : bones) {
            auto& name = bone.name;
            auto absolute_bone_transform = get_absolute_node_transform(name);
            // NOTE(panmar): Skinning equation; 'inv_root_transform' is
            // additional element here since 'absolute_bone_transform' is
            // computed with respect to root node whereas
            // bone.part_to_bone_transform is compute with respect to skeleton
            // root (armature) - which is a child of root; usually
            // root_transform is identity and it does not matter; if it is not -
            // it matters a lot, hence the multiplication
            pallete.push_back(inv_root_transform * absolute_bone_transform *
                              bone.part_to_bone_transform);
        }
        return pallete;
    }

    void compute_bounding_sphere() {
        if (model_parts.empty()) {
            return;
        }

        for (auto& part : model_parts) {
            part.bounding_sphere = BoundingSphere::from_points(part.positions);
        }

        auto points_count = 0U;
        glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
        for (auto& node : current_model_graph) {
            auto absolute_transform = get_absolute_node_transform(node.name);
            for (int i = 0; i < node.model_parts.size(); ++i) {
                auto& part = model_parts[node.model_parts[i]];
                auto skinning_palette = get_skinning_palette(part.bones);
                if (skinning_palette.empty()) {
                    for (auto& pos : part.positions) {
                        auto pos_ws = absolute_transform * glm::vec4(pos, 1.f);
                        center += glm::vec3(pos_ws);
                        ++points_count;
                    }
                } else {
                    for (size_t i = 0; i < part.positions.size(); ++i) {
                        auto pos = part.positions[i];
                        auto blend_indices = part.joints[i];
                        auto blend_weights = part.weights[i];
                        glm::mat4 xform = skinning_palette[blend_indices.v[0]] *
                                          blend_weights[0];
                        xform += skinning_palette[blend_indices.v[1]] *
                                 blend_weights[1];
                        xform += skinning_palette[blend_indices.v[2]] *
                                 blend_weights[2];
                        xform += skinning_palette[blend_indices.v[3]] *
                                 blend_weights[3];
                        glm::vec4 pos_ws =
                            xform * glm::vec4(pos.x, pos.y, pos.z, 1.f);
                        center += glm::vec3(pos_ws);
                        ++points_count;
                    }
                }
            }
        }

        center /= points_count;
        auto radius = 0.f;
        for (auto& node : current_model_graph) {
            auto absolute_transform = get_absolute_node_transform(node.name);
            for (int i = 0; i < node.model_parts.size(); ++i) {
                for (auto& pos : model_parts[i].positions) {
                    auto pos_ws = absolute_transform * glm::vec4(pos, 1.f);
                    radius = std::max(radius,
                                      glm::distance(center, glm::vec3(pos_ws)));
                }
            }
        }

        bounding_sphere.center = center;
        bounding_sphere.radius = radius;

        printf("Bounding sphere: (c = (%f, %f, %f), r = %f\n", center.x,
               center.y, center.z, radius);
    }

    // NOTE(panmar): This is transform for the graph root; Usually identity;
    // But if not, this is important to neutralize this transform when computing
    // GetAbsoluteNodeTransform() during GetSkinningPalette() since bone
    // transforms are usually computed against armature root, not a graph root;
    glm::mat4 root_transform = glm::mat4(1.f);
    std::vector<ModelPart> model_parts;
    std::vector<ModelNode> bind_model_graph;
    std::vector<ModelNode> current_model_graph;

    using Animations = std::map<std::string, std::vector<AnimationChannel>>;
    using AnimationsDuration = std::map<std::string, f32>;
    Animations animations;
    AnimationsDuration animations_duration;

    BoundingSphere bounding_sphere;
};

class AnimationController {
public:
    AnimationController(ModelData& model_data) : data(model_data) {}

    bool play(const std::string& animation, bool looped = true) {
        auto it = data.animations.find(animation);
        if (it != data.animations.end()) {
            current_animation = animation;
            current_frame = 0.0f;
            this->looped = looped;
            paused = false;
            animation_ended = false;
            return true;
        }
        return false;
    }

    void pause() { paused = true; }

    void resume() { paused = false; }

    void stop() {
        current_animation = "";
        current_frame = 0.f;
        paused = false;
        animation_ended = true;
    }

    void update(f32 elapsed_seconds) {
        if (paused || current_animation.empty()) {
            return;
        }

        // TODO(panmar): Base increment on elapsing time
        current_frame += elapsed_seconds * 16.f;

        auto it = data.animations.find(current_animation);
        if (it == data.animations.end()) {
            return;
        }

        const std::vector<AnimationChannel>& animation_channels = it->second;

        for (auto channel_it = std::begin(animation_channels);
             channel_it != std::end(animation_channels); ++channel_it) {
            const std::string& channel_name = channel_it->name;
            const std::map<u32, AnimationTransform>& channel_keys =
                channel_it->keys;

            if (!channel_keys.empty()) {
                if (1 == channel_keys.size()) {
                    glm::mat4 channel_transform =
                        channel_keys.begin()->second.get_transform();
                    set_node_transform(channel_name, channel_transform);
                } else {
                    auto start_key_it =
                        channel_keys.lower_bound((u32)ceil(current_frame));

                    if (start_key_it == channel_keys.end()) {
                        if (looped) {
                            start_key_it = channel_keys.begin();
                            current_frame = 0.0f;
                        } else {
                            animation_ended = true;
                        }
                    }

                    if (start_key_it != channel_keys.begin()) {
                        --start_key_it;
                    }

                    u32 from_key = start_key_it->first;

                    glm::vec3 start_position = start_key_it->second.position;
                    glm::quat start_rotation = start_key_it->second.rotation;
                    glm::vec3 start_scale = start_key_it->second.scale;

                    ++start_key_it;
                    if (start_key_it == channel_keys.end()) {
                        start_key_it = channel_keys.begin();
                    }

                    u32 to_key = start_key_it->first;

                    glm::vec3 end_position = start_key_it->second.position;
                    glm::quat end_rotation = start_key_it->second.rotation;
                    glm::vec3 end_scale = start_key_it->second.scale;

                    f32 factor = 1.0f;
                    if (to_key != from_key) {
                        factor = std::max(0.0f, current_frame - from_key) /
                                 (f32)(to_key - from_key);
                    }

                    glm::vec3 current_position =
                        glm::lerp(start_position, end_position, factor);
                    glm::quat current_rotation =
                        glm::slerp(start_rotation, end_rotation, factor);
                    glm::vec3 current_scale =
                        glm::lerp(start_scale, end_scale, factor);

                    auto identity = glm::mat4(1.f);
                    auto translate = glm::translate(identity, current_position);
                    auto rotation = glm::toMat4(current_rotation);
                    auto scale = glm::scale(identity, current_scale);

                    set_node_transform(channel_name,
                                       translate * rotation * scale);
                }
            }

            // auto start_key_it =
            // channel_keys.lower_bound((u32)ceil(current_frame));
        }
    }

    std::string current_animation = "";
    f32 current_frame = 0.f;
    bool looped = false;
    bool paused = false;
    bool animation_ended = false;

private:
    void set_node_transform(const std::string& node_name,
                            const glm::mat4& transform) {
        for (auto& model_node : data.current_model_graph) {
            if (node_name == model_node.name) {
                model_node.transform = transform;
                return;
            }
        }

        assert(false);
    }

    ModelData& data;
};

class Model {
public:
    void update(f32 elapsed_time) {
        if (animation_controller) {
            animation_controller->update(elapsed_time);
        }
    }

    ModelData data;
    unique_ptr<AnimationController> animation_controller;
};
