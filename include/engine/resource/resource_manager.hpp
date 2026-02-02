#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <string_view>

namespace engine::resource {

class ResourceManager final {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // --- Texture ---
    sf::Texture* load_texture(std::string_view file);
    sf::Texture* get_texture(std::string_view file);
    void unload_texture(std::string_view file);
    void clear_textures();

    // --- SoundBuffer ---
    sf::SoundBuffer* load_sound(std::string_view file);
    sf::SoundBuffer* get_sound(std::string_view file);
    void unload_sound(std::string_view file);
    void clear_sounds();

    // --- Music ---
    sf::Music* load_music(std::string_view file);
    sf::Music* get_music(std::string_view file);
    void unload_music(std::string_view file);
    void clear_musics();

    // --- Font ---
    sf::Font* load_font(std::string_view file);
    sf::Font* get_font(std::string_view file);
    void unload_font(std::string_view file);
    void clear_fonts();

    // --- All ---
    void clear_all();

private:
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures_;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> sounds_;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> musics_;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts_;
};

} // namespace engine::resource
