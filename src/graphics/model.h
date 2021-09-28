#pragma once

#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include "common.h"
#include "graphics/geometry.h"
#include "resource.h"

struct ModelPart {
    ModelPart(const string& name, const Geometry& geometry,
              const mat4& transform)
        : name(name), geometry(geometry), transform(transform) {}
    string name;
    Geometry geometry;
    mat4 transform = mat4(1.f);
};

struct ModelData {
    vector<ModelPart> parts;
};

class GltfModelImporter {
public:
    static ModelData import(const Path& path) {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        string error;
        string warn;

        if (!loader.LoadBinaryFromFile(&model, &error, &warn, path.string())) {
            throw PlayGlException(error);
        }

        if (!warn.empty()) {
            fmt::print("GltfModelImporter: {}", warn);
        }

        if (model.nodes.empty()) {
            throw PlayGlException("Model has no nodes");
        }

        ModelData model_data;

        vector<mat4> parent_transforms(model.nodes.size(), mat4(1.f));
        for (i32 i = model.nodes.size() - 1; i >= 0; --i) {
            auto& node = model.nodes[i];
            if (node.mesh == -1) {
                continue;
            }

            auto absolute_node_transform =
                compute_absolute_transform(node, i, parent_transforms);

            for (auto& child_node_index : node.children) {
                if (child_node_index >= i) {
                    throw PlayGlException(
                        "Model nodes are not sorted topologically");
                }
                parent_transforms[child_node_index] = absolute_node_transform;
            }

            auto& model_part = model_data.parts.emplace_back(
                node.name, create_geometry_from_mesh(node.mesh, model),
                absolute_node_transform);
        }

        return model_data;
    }

    static mat4 compute_absolute_transform(
        const tinygltf::Node& node, i32 node_index,
        const vector<mat4>& parent_transforms) {
        mat4 local_transform = mat4(1.f);

        if (!node.translation.empty()) {
            local_transform =
                glm::translate(vec3(static_cast<f32>(node.translation[0]),
                                    static_cast<f32>(node.translation[1]),
                                    static_cast<f32>(node.translation[2])));
        }

        if (!node.rotation.empty()) {
            local_transform =
                local_transform *
                glm::rotate(static_cast<f32>(node.rotation[0]),
                            vec3(static_cast<f32>(node.rotation[1]),
                                 static_cast<f32>(node.rotation[2]),
                                 static_cast<f32>(node.rotation[3])));
        }

        if (!node.scale.empty()) {
            local_transform = local_transform *
                              glm::scale(vec3(static_cast<f32>(node.scale[0]),
                                              static_cast<f32>(node.scale[1]),
                                              static_cast<f32>(node.scale[2])));
        }

        return local_transform * parent_transforms[node_index];
    }

    static Geometry create_geometry_from_mesh(i32 mesh_id,
                                              const tinygltf::Model& model) {
        Geometry geometry;
        auto& mesh = model.meshes[mesh_id];
        for (const auto& primitive : mesh.primitives) {
            const auto& indices_view_index = primitive.indices;
            auto& indices_buffer_index =
                model.bufferViews[indices_view_index].buffer;
            auto& indices_byte_offset =
                model.bufferViews[indices_view_index].byteOffset;
            auto& indices_byte_length =
                model.bufferViews[indices_view_index].byteLength;
            auto& indices_byte_stride =
                model.bufferViews[indices_view_index].byteStride;
            auto begin = reinterpret_cast<const u16*>(
                &model.buffers[indices_buffer_index].data[indices_byte_offset]);
            auto end = reinterpret_cast<const u16*>(
                &model.buffers[indices_buffer_index]
                     .data[indices_byte_offset + indices_byte_length]);
            auto indices_u16 = vector<u16>(begin, end);
            geometry.indices =
                vector<u32>(indices_u16.begin(), indices_u16.end());

            for (const auto& named_attribute : primitive.attributes) {
                auto& name = named_attribute.first;
                auto& attrib_index = named_attribute.second;
                auto& buffer_index = model.bufferViews[attrib_index].buffer;
                auto& byte_offset = model.bufferViews[attrib_index].byteOffset;
                auto& byte_length = model.bufferViews[attrib_index].byteLength;
                auto& byte_stride = model.bufferViews[attrib_index].byteStride;
                if (name == "POSITION") {
                    auto begin = reinterpret_cast<const vec3*>(
                        &model.buffers[buffer_index].data[byte_offset]);
                    auto end = reinterpret_cast<const vec3*>(
                        &model.buffers[buffer_index]
                             .data[byte_offset + byte_length]);
                    geometry.positions = vector<vec3>(begin, end);
                } else if (name == "NORMAL") {
                    auto begin = reinterpret_cast<const vec3*>(
                        &model.buffers[buffer_index].data[byte_offset]);
                    auto end = reinterpret_cast<const vec3*>(
                        &model.buffers[buffer_index]
                             .data[byte_offset + byte_length]);
                    geometry.normals = vector<vec3>(begin, end);
                } else if (name == "TEXCOORD_0") {
                    auto begin = reinterpret_cast<const vec2*>(
                        &model.buffers[buffer_index].data[byte_offset]);
                    auto end = reinterpret_cast<const vec2*>(
                        &model.buffers[buffer_index]
                             .data[byte_offset + byte_length]);
                    geometry.texcoords = vector<vec2>(begin, end);
                } else {
                    // throw PlayGlException("Unsupported attribute");
                }
            }
        }
        return geometry;
    }
};

class Model : public LazyResource<ModelData> {
public:
    static Model from_file(const Path& path) { return Model(path); }

    const ModelPart& operator[](const string& name) const {
        for (auto& part : resource().parts) {
            if (part.name == name) {
                return part;
            }
        }
        throw PlayGlException(
            fmt::format("Could not find geometry `{}`", name));
    }

    auto cbegin() {
        return resource().parts.cbegin();
    }

    auto cend() {
        return resource().parts.cend();
    }

private:
    Model(const Path& path) : LazyResource(resource_deleter), path(path) {}

    virtual ModelData create_resource() const override {
        return GltfModelImporter::import(path);
    }

    static void resource_deleter(ModelData& resource) {}

    const Path path;
};
