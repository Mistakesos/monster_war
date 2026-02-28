#include "engine/resource/resource_manager.hpp"
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace engine::resource {

// ======================== 内部逻辑提取 ========================
// 这里的逻辑遵循：有路径就尝试加载，没路径就只查缓存

void ResourceManager::load_resource(std::string_view file_path) {
    std::filesystem::path path(file_path);
    if (!std::filesystem::exists(path)) {
        spdlog::warn("资源映射文件不存在: {}", file_path);
        return;
    }
    std::ifstream file(path);
    nlohmann::json json;
    file >> json;
    try {
        if (json.contains("sound")) {
            for (const auto& [key, value] : json["sound"].items()) {
                load_sound(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("music")) {
            for (const auto& [key, value] : json["music"].items()) {
                load_music(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("texture")) {
            for (const auto& [key, value] : json["texture"].items()) {
                load_texture(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("font")) {
            for (const auto& [key, value] : json["font"].items()) {
                load_font(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }   
    } catch (const nlohmann::json::exception& e) {
        spdlog::error("加载资源文件失败: {}", e.what());
    }
}


// ---------------- Texture ----------------
sf::Texture* ResourceManager::load_texture(entt::id_type id, std::string_view file_path) {
    if (textures_.contains(id)) return textures_[id].get();

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(std::string(file_path))) {
        spdlog::error("ResourceManager: 无法加载贴图 '{}'", file_path);
        return nullptr;
    }
    spdlog::debug("ResourceManager: 贴图加载成功 '{}'", file_path);
    return textures_.emplace(id, std::move(texture)).first->second.get();
}

sf::Texture* ResourceManager::get_texture(entt::id_type id, std::string_view file_path) {
    if (textures_.contains(id)) return textures_[id].get();
    if (file_path.empty()) return nullptr; // 没缓存且没路径，静默返回

    spdlog::info("ResourceManager: 缓存未命中，开始加载贴图 '{}'", file_path);
    return load_texture(id, file_path);
}

// ---------------- SoundBuffer ----------------
sf::SoundBuffer* ResourceManager::load_sound(entt::id_type id, std::string_view file_path) {
    if (sounds_.contains(id)) return sounds_[id].get();

    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(std::string(file_path))) {
        spdlog::error("ResourceManager: 无法加载音效 '{}'", file_path);
        return nullptr;
    }
    spdlog::debug("ResourceManager: 音效加载成功 '{}'", file_path);
    return sounds_.emplace(id, std::move(buffer)).first->second.get();
}

sf::SoundBuffer* ResourceManager::get_sound(entt::id_type id, std::string_view file_path) {
    if (sounds_.contains(id)) return sounds_[id].get();
    if (file_path.empty()) return nullptr;

    spdlog::info("ResourceManager: 缓存未命中，开始加载音效 '{}'", file_path);
    return load_sound(id, file_path);
}

// ---------------- Music ----------------
sf::Music* ResourceManager::load_music(entt::id_type id, std::string_view file_path) {
    if (musics_.contains(id)) return musics_[id].get();

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(std::string(file_path))) {
        spdlog::error("ResourceManager: 无法打开音乐文件 '{}'", file_path);
        return nullptr;
    }
    spdlog::debug("ResourceManager: 音乐关联成功 '{}'", file_path);
    return musics_.emplace(id, std::move(music)).first->second.get();
}

sf::Music* ResourceManager::get_music(entt::id_type id, std::string_view file_path) {
    if (musics_.contains(id)) return musics_[id].get();
    if (file_path.empty()) return nullptr;

    spdlog::info("ResourceManager: 缓存未命中，开始打开音乐 '{}'", file_path);
    return load_music(id, file_path);
}

// ---------------- Font ----------------
sf::Font* ResourceManager::load_font(entt::id_type id, std::string_view file_path) {
    if (fonts_.contains(id)) return fonts_[id].get();

    auto font = std::make_unique<sf::Font>();
    if (!font->openFromFile(std::string(file_path))) {
        spdlog::error("ResourceManager: 无法加载字体 '{}'", file_path);
        return nullptr;
    }
    spdlog::debug("ResourceManager: 字体加载成功 '{}'", file_path);
    return fonts_.emplace(id, std::move(font)).first->second.get();
}

sf::Font* ResourceManager::get_font(entt::id_type id, std::string_view file_path) {
    if (fonts_.contains(id)) return fonts_[id].get();
    if (file_path.empty()) return nullptr;

    spdlog::info("ResourceManager: 缓存未命中，开始加载字体 '{}'", file_path);
    return load_font(id, file_path);
}

// ======================== Hashed String 重载 ========================

sf::Texture* ResourceManager::get_texture(entt::hashed_string hs) { return get_texture(hs.value(), hs.data()); }
sf::SoundBuffer* ResourceManager::get_sound(entt::hashed_string hs)   { return get_sound(hs.value(), hs.data()); }
sf::Music* ResourceManager::get_music(entt::hashed_string hs)   { return get_music(hs.value(), hs.data()); }
sf::Font* ResourceManager::get_font(entt::hashed_string hs)    { return get_font(hs.value(), hs.data()); }

sf::Texture* ResourceManager::load_texture(entt::hashed_string hs) { return load_texture(hs.value(), hs.data()); }
sf::SoundBuffer* ResourceManager::load_sound(entt::hashed_string hs)   { return load_sound(hs.value(), hs.data()); }
sf::Music* ResourceManager::load_music(entt::hashed_string hs)   { return load_music(hs.value(), hs.data()); }
sf::Font* ResourceManager::load_font(entt::hashed_string hs)    { return load_font(hs.value(), hs.data()); }


// ======================== 卸载逻辑 (保持现状) ========================

void ResourceManager::unload_texture(entt::id_type id) { if (textures_.erase(id)) spdlog::debug("Unloaded Texture {}", id); }
void ResourceManager::unload_sound(entt::id_type id)   { if (sounds_.erase(id))   spdlog::debug("Unloaded Sound {}", id); }
void ResourceManager::unload_music(entt::id_type id)   { if (musics_.erase(id))   spdlog::debug("Unloaded Music {}", id); }
void ResourceManager::unload_font(entt::id_type id)    { if (fonts_.erase(id))    spdlog::debug("Unloaded Font {}", id); }

void ResourceManager::clear_textures() { textures_.clear(); }
void ResourceManager::clear_sounds()   { sounds_.clear(); }
void ResourceManager::clear_musics()   { musics_.clear(); }
void ResourceManager::clear_fonts()    { fonts_.clear(); }
void ResourceManager::clear_all()      { clear_textures(); clear_sounds(); clear_musics(); clear_fonts(); }

} // namespace engine::resource