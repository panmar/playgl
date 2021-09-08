#pragma once

#include "common.h"
#include "system.h"
#include "model.h"
#include "model_importer.h"
#include "graphics/opengl/shader.h"

namespace Content {

const string data_dir = "data/";
const string shaders_dir = "shaders/";

Model& model(const string& name) {
    static unordered_map<string, unique_ptr<Model>> models;

    auto it = models.find(name);
    if (it != models.end()) {
        return *(it->second.get());
    }

    AssimpModelImporter importer;
    auto model = std::make_unique<Model>();

    string path = data_dir + name + ".fbx";

    model->data = importer.import(path);
    models.insert({name, std::move(model)});
    return *(models.find(name)->second.get());
}

Shader& shader(const string& vs_filepath, const string& fs_filepath) {
    static unordered_map<string, unique_ptr<Shader>> shaders;

    string name = vs_filepath + "__" + fs_filepath;

    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return *(it->second.get());
    }

    string vs = read_file(data_dir + shaders_dir + vs_filepath);
    string fs = read_file(data_dir + shaders_dir + fs_filepath);

    auto shader = std::make_unique<Shader>(vs, fs);
    shaders.insert({name, std::move(shader)});
    return *(shaders.find(name)->second.get());
}

// Texture& texture(const string& name) {
// static unordered_map<string, unique_ptr<Texture>> textures;
//}

}  // namespace Content

#define MODEL(path) Content::model((path))
#define SHADER(vs_path, fs_path) Content::shader((vs_path), (fs_path))
// #define TEXTURE(str) Content::texture((str))