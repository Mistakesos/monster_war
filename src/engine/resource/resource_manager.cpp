#include "engine/resource/resource_manager.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource {
// ---------------- Texture ----------------
sf::Texture* ResourceManager::load_texture(std::string_view file) {
    auto key = std::string(file);
    if (textures_.contains(key)) return textures_[key].get();

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(key)) {
        spdlog::error("Failed to load Texture '{}'", key);
        return nullptr;
    }
    spdlog::debug("Loaded Texture '{}'", key);
    return textures_.emplace(key, std::move(texture)).first->second.get();
}

sf::Texture* ResourceManager::get_texture(std::string_view file) {
    auto key = std::string(file);
    if (textures_.contains(key)) return textures_[key].get();
    spdlog::warn("Texture '{}' not found, loading...", key);
    return load_texture(key);
}

void ResourceManager::unload_texture(std::string_view file) {
    textures_.erase(std::string(file));
    spdlog::debug("Unloaded Texture '{}'", file);
}

void ResourceManager::clear_textures() {
    textures_.clear();
}

// ---------------- SoundBuffer ----------------
sf::SoundBuffer* ResourceManager::load_sound(std::string_view file) {
    auto key = std::string(file);
    if (sounds_.contains(key)) return sounds_[key].get();

    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(key)) {
        spdlog::error("Failed to load SoundBuffer '{}'", key);
        return nullptr;
    }
    spdlog::debug("Loaded SoundBuffer '{}'", key);
    return sounds_.emplace(key, std::move(buffer)).first->second.get();
}

sf::SoundBuffer* ResourceManager::get_sound(std::string_view file) {
    auto key = std::string(file);
    if (sounds_.contains(key)) return sounds_[key].get();
    spdlog::warn("SoundBuffer '{}' not found, loading...", key);
    return load_sound(key);
}

void ResourceManager::unload_sound(std::string_view file) {
    sounds_.erase(std::string(file));
    spdlog::debug("Unloaded SoundBuffer '{}'", file);
}

void ResourceManager::clear_sounds() {
    sounds_.clear();
}

// ---------------- Music ----------------
sf::Music* ResourceManager::load_music(std::string_view file) {
    auto key = std::string(file);
    if (musics_.contains(key)) return musics_[key].get();

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(key)) {
        spdlog::error("Failed to load Music '{}'", key);
        return nullptr;
    }
    spdlog::debug("Loaded Music '{}'", key);
    return musics_.emplace(key, std::move(music)).first->second.get();
}

sf::Music* ResourceManager::get_music(std::string_view file) {
    auto key = std::string(file);
    if (musics_.contains(key)) return musics_[key].get();
    spdlog::warn("Music '{}' not found, loading...", key);
    return load_music(key);
}

void ResourceManager::unload_music(std::string_view file) {
    musics_.erase(std::string(file));
    spdlog::debug("Unloaded music '{}'", file);
}

void ResourceManager::clear_musics() {
    musics_.clear();
}

// ---------------- Font ----------------
sf::Font* ResourceManager::load_font(std::string_view file) {
    auto key = std::string(file);
    if (fonts_.contains(key)) return fonts_[key].get();

    auto font = std::make_unique<sf::Font>();
    if (!font->openFromFile(key)) {
        spdlog::error("Failed to load Font '{}'", key);
        return nullptr;
    }
    spdlog::debug("Loaded Font '{}'", key);
    return fonts_.emplace(key, std::move(font)).first->second.get();
}

sf::Font* ResourceManager::get_font(std::string_view file) {
    auto key = std::string(file);
    if (fonts_.contains(key)) return fonts_[key].get();
    spdlog::warn("Font '{}' not found, loading...", key);
    return load_font(key);
}

void ResourceManager::unload_font(std::string_view file) {
    fonts_.erase(std::string(file));
    spdlog::debug("Unloaded Font '{}'", file);
}

void ResourceManager::clear_fonts() {
    fonts_.clear();
}

// ---------------- All ----------------
void ResourceManager::clear_all() {
    clear_textures();
    clear_sounds();
    clear_musics();
    clear_fonts();
}
} // namespace engine::resource
