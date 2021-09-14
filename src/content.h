#pragma once

#include "common.h"
#include "system.h"
#include "model.h"
#include "model_importer.h"
#include "graphics/opengl/shader.h"

namespace Content {

const std::filesystem::path data_dir = "data/";
const std::filesystem::path model_dir = "models/";
const std::filesystem::path shaders_dir = "shaders/";

Model& model(const string& name) {
    static unordered_map<string, unique_ptr<Model>> models;

    auto it = models.find(name);
    if (it != models.end()) {
        return *(it->second.get());
    }

    AssimpModelImporter importer;
    auto model = std::make_unique<Model>();

    auto path = data_dir / model_dir / (name + ".fbx");

    model->data = importer.import(path);
    models.insert({name, std::move(model)});
    return *(models.find(name)->second.get());
}

Shader& shader(const string& vs_name, const string& fs_name) {
    static unordered_map<string, unique_ptr<Shader>> shaders;

    auto name = vs_name + "+" + fs_name;

    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return *(it->second.get());
    }

    string vs = read_file(data_dir / shaders_dir / vs_name);
    string fs = read_file(data_dir / shaders_dir / fs_name);

    auto shader = std::make_unique<Shader>(vs, fs);
    shaders.insert({name, std::move(shader)});
    return *(shaders.find(name)->second.get());
}

// Texture& texture(const string& name) {
// static unordered_map<string, unique_ptr<Texture>> textures;
//}

}  // namespace Content

#define MODEL(path) Content::model((path))
#define SHADER(vs_path, fs_path) Content::shader((vs_name), (fs_name))
// #define TEXTURE(str) Content::texture((str))