#pragma once

#include "common.h"
#include "graphics/model.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

class Content {
public:
    Content(std::filesystem::path& data_dir) {
        for (const auto& entry :
             std::filesystem::recursive_directory_iterator{data_dir}) {
            if (entry.is_regular_file()) {
                resource_filepaths.push_back(entry.path());
            }
        }
    }

    Model& model(const string& id) {
        auto it = id_to_models.find(id);
        if (it != id_to_models.end()) {
            return it->second;
        }

        auto path_it =
            std::find_if(resource_filepaths.begin(), resource_filepaths.end(),
                         [&id](const std::filesystem::path& p) {
                             return id == p.filename();
                         });

        if (path_it == resource_filepaths.end()) {
            string error = fmt::format("Cannot find resource {}", id);
            throw PlayGlException(error);
        }

        return id_to_models
            .insert({id, std::move(Model::from_file(*path_it))})
            .first->second;
    }

    Shader& shader(const string& vs_id, const string& fs_id) {
        auto id = vs_id + "+" + fs_id;

        auto it = id_to_shaders.find(id);
        if (it != id_to_shaders.end()) {
            return it->second;
        }

        auto vs_path_it =
            std::find_if(resource_filepaths.begin(), resource_filepaths.end(),
                         [&vs_id](const std::filesystem::path& p) {
                             return vs_id == p.filename();
                         });

        if (vs_path_it == resource_filepaths.end()) {
            string error = fmt::format("Cannot find resource {}", vs_id);
            throw PlayGlException(error);
        }

        auto fs_path_it =
            std::find_if(resource_filepaths.begin(), resource_filepaths.end(),
                         [&fs_id](const std::filesystem::path& p) {
                             return fs_id == p.filename();
                         });

        if (fs_path_it == resource_filepaths.end()) {
            string error = fmt::format("Cannot find resource {}", fs_id);
            throw PlayGlException(error);
        }

        auto shader = Shader::from_file(*vs_path_it, *fs_path_it);
        return id_to_shaders.insert({id, std::move(shader)}).first->second;
    }

    Shader& shader(const string& id) { return shader(id, id); }

    Texture& texture(const string& id) {
        auto it = id_to_textures.find(id);
        if (it != id_to_textures.end()) {
            return it->second;
        }

        auto path_it =
            std::find_if(resource_filepaths.begin(), resource_filepaths.end(),
                         [&id](const std::filesystem::path& p) {
                             return id == p.filename();
                         });

        if (path_it == resource_filepaths.end()) {
            string error = fmt::format("Cannot find resource {}", id);
            throw PlayGlException(error);
        }

        return id_to_textures
            .insert({id, std::move(Texture::from_file(*path_it))})
            .first->second;
    }

private:
    vector<std::filesystem::path> resource_filepaths;

    unordered_map<string, Model> id_to_models;
    unordered_map<string, Shader> id_to_shaders;
    unordered_map<string, Texture> id_to_textures;
};
