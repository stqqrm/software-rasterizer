#include "asset_manager.h"
#include <iostream>

namespace core {

std::unordered_map<std::string, texture> asset_manager::_texture_cache;
std::unordered_map<std::string, model>   asset_manager::_model_cache;
std::unordered_map<std::string, font>    asset_manager::_font_cache;

static std::string make_key(const fs::path& file_path) {
    fs::path rel;
    if (file_path.string().starts_with(ASSET_DIR))
        rel = fs::relative(file_path, ASSET_DIR);
    else
        rel = file_path;
    std::string key = rel.string();
    std::replace(key.begin(), key.end(), '\\', '/');
    return key;
}

void asset_manager::Init() {
    for (auto& entry : fs::recursive_directory_iterator(ASSET_DIR)) {
        if (!entry.is_regular_file()) continue;
        std::string key = make_key(entry.path());
        if (LoadTexture(entry.path(), key).has_value()
            || LoadModel(entry.path(), key).has_value()
            || LoadFont(entry.path(), key).has_value()) {
            printf("Cached %s\n", key.c_str());
        } else {
            std::cerr << "Failed to cache " << entry.path().string() << std::endl;
        }
    }
}

std::optional<std::reference_wrapper<model>> asset_manager::LoadModel(const fs::path& file_path, const std::string& key) {
    if (_model_cache.count(key)) return _model_cache[key];
    model m;
    if (m.Load(file_path)) {
        _model_cache[key] = std::move(m);
        return _model_cache[key];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<texture>> asset_manager::LoadTexture(const fs::path& file_path, const std::string& key) {
    if (_texture_cache.count(key)) return _texture_cache[key];
    texture tex;
    if (tex.Load(file_path.string())) {
        _texture_cache[key] = std::move(tex);
        return _texture_cache[key];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<font>> asset_manager::LoadFont(const fs::path& file_path, const std::string& key) {
    if (_font_cache.count(key)) return _font_cache[key];
    font f;
    if (f.Load(file_path.string())) {
        _font_cache[key] = std::move(f);
        return _font_cache[key];
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<model>> asset_manager::GetModel(const std::string& name) {
    std::string key = make_key(fs::path(name));
    auto it = _model_cache.find(key);
    if (it != _model_cache.end()) return it->second;
    std::cerr << "Model not found in cache: " << name << std::endl;
    return std::nullopt;
}

std::optional<std::reference_wrapper<texture>> asset_manager::GetTexture(const std::string& name) {
    std::string key = make_key(fs::path(name));
    auto it = _texture_cache.find(key);
    if (it != _texture_cache.end()) return it->second;
    std::cerr << "Texture not found in cache: " << name << std::endl;
    return std::nullopt;
}

std::optional<std::reference_wrapper<font>> asset_manager::GetFont(const std::string& name) {
    std::string key = make_key(fs::path(name));
    auto it = _font_cache.find(key);
    if (it != _font_cache.end()) return it->second;
    std::cerr << "Font not found in cache: " << name << std::endl;
    return std::nullopt;
}

} // namespace core
