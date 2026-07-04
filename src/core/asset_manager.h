#pragma once
#include "inc.h"
#include "model.h"

#define ASSET_DIR "assets/"

namespace core {

class asset_manager {
public:
    asset_manager() = default;
    ~asset_manager() = default;
    static void Init();
    static std::optional<std::reference_wrapper<model>>   LoadModel(const fs::path& file_path, const std::string& key);
    static std::optional<std::reference_wrapper<texture>> LoadTexture(const fs::path& file_path, const std::string& key);
    static std::optional<std::reference_wrapper<font>>    LoadFont(const fs::path& file_path, const std::string& key);
    static std::optional<std::reference_wrapper<model>>   GetModel(const std::string& name);
    static std::optional<std::reference_wrapper<texture>> GetTexture(const std::string& name);
    static std::optional<std::reference_wrapper<font>>    GetFont(const std::string& name);
private:
    static std::unordered_map<std::string, texture> _texture_cache;
    static std::unordered_map<std::string, model>   _model_cache;
    static std::unordered_map<std::string, font>    _font_cache;
};

} // namespace core
