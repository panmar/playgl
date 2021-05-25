#pragma once

#include "common.h"
#include "model.h"
#include "model_importer.h"

namespace Content {

const string data_dir = "data/";

unordered_map<string, unique_ptr<Model>> models;
// unordered_map<string, unique_ptr<Shader>> shaders;
// unordered_map<string, unique_ptr<Texture>> textures;

Model& model(const string& name) {
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

// Shader& shader(const string& name) {}
// Texture& texture(const string& name) {}

}  // namespace Content

#define MODEL(str) Content::model((str))