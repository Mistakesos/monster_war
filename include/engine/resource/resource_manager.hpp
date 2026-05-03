#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string_view>
#include "entt/core/fwd.hpp"
#include <spdlog/fmt/ostr.h>

namespace engine::resource {

class ResourceManager final {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // 加载资源
    void load_resource(std::string_view file_path);
    
    // --- Texture ---
    /**
     * @brief 加载纹理，若已缓存则直接返回缓存版本。
     * @param id 纹理的唯一标识符（entt::hashed_string 值）
     * @param file_path 纹理文件路径
     * @return 纹理指针，加载失败返回 nullptr
     */
    sf::Texture* load_texture(entt::id_type id, std::string_view file_path);

    /** @overload 使用 entt::hashed_string 作为标识符和文件路径 */
    sf::Texture* load_texture(entt::hashed_string str_hs);

    /**
     * @brief 获取纹理。若未缓存且提供了 file_path，则自动加载；否则返回 nullptr。
     * @param id 纹理的唯一标识符
     * @param file_path 可选，用于自动加载
     * @return 纹理指针，不存在且无法加载时返回 nullptr
     */
    sf::Texture* get_texture(entt::id_type id, std::string_view file_path = "");

    /** @overload 使用 entt::hashed_string 作为标识符（不自动加载） */
    sf::Texture* get_texture(entt::hashed_string str_hs);

    /** @brief 卸载指定标识符的纹理 */
    void unload_texture(entt::id_type id);

    /** @brief 清空所有纹理 */
    void clear_textures();

    // --- SoundBuffer ---
    /**
     * @brief 加载音效缓冲区，若已缓存则直接返回。
     * @param id 音效标识符
     * @param file_path 音效文件路径
     * @return SoundBuffer 指针，失败返回 nullptr
     */
    sf::SoundBuffer* load_sound(entt::id_type id, std::string_view file_path);

    /** @overload 使用 entt::hashed_string 作为标识符和文件路径 */
    sf::SoundBuffer* load_sound(entt::hashed_string str_hs);

    /**
     * @brief 获取音效缓冲区。若未缓存且提供了 file_path，则自动加载；否则返回 nullptr。
     * @param id 音效标识符
     * @param file_path 可选，用于自动加载
     * @return SoundBuffer 指针，不存在且无法加载时返回 nullptr
     */
    sf::SoundBuffer* get_sound(entt::id_type id, std::string_view file_path = "");

    /** @overload 使用 entt::hashed_string 作为标识符（不自动加载） */
    sf::SoundBuffer* get_sound(entt::hashed_string str_hs);

    /** @brief 卸载指定标识符的音效缓冲区 */
    void unload_sound(entt::id_type id);

    /** @brief 清空所有音效缓冲区 */
    void clear_sounds();

    // --- Music ---
    /**
     * @brief 加载音乐流（流式播放），若已缓存则直接返回。
     * @param id 音乐标识符
     * @param file_path 音乐文件路径
     * @return Music 指针，失败返回 nullptr
     */
    sf::Music* load_music(entt::id_type id, std::string_view file_path);

    /** @overload 使用 entt::hashed_string 作为标识符和文件路径 */
    sf::Music* load_music(entt::hashed_string str_hs);

    /**
     * @brief 获取音乐。若未缓存且提供了 file_path，则自动加载；否则返回 nullptr。
     * @param id 音乐标识符
     * @param file_path 可选，用于自动加载
     * @return Music 指针，不存在且无法加载时返回 nullptr
     */
    sf::Music* get_music(entt::id_type id, std::string_view file_path = "");

    /** @overload 使用 entt::hashed_string 作为标识符（不自动加载） */
    sf::Music* get_music(entt::hashed_string str_hs);

    /** @brief 卸载指定标识符的音乐 */
    void unload_music(entt::id_type id);

    /** @brief 清空所有音乐 */
    void clear_musics();

    // --- Font ---
    /**
     * @brief 加载字体，若已缓存则直接返回。
     * @param id 字体标识符
     * @param file_path 字体文件路径
     * @return Font 指针，失败返回 nullptr
     */
    sf::Font* load_font(entt::id_type id, std::string_view file_path);

    /** @overload 使用 entt::hashed_string 作为标识符和文件路径 */
    sf::Font* load_font(entt::hashed_string str_hs);

    /**
     * @brief 获取字体。若未缓存且提供了 file_path，则自动加载；否则返回 nullptr。
     * @param id 字体标识符
     * @param file_path 可选，用于自动加载
     * @return Font 指针，不存在且无法加载时返回 nullptr
     */
    sf::Font* get_font(entt::id_type id, std::string_view file_path = "");

    /** @overload 使用 entt::hashed_string 作为标识符（不自动加载） */
    sf::Font* get_font(entt::hashed_string str_hs);

    /** @brief 卸载指定标识符的字体 */
    void unload_font(entt::id_type id);

    /** @brief 清空所有字体 */
    void clear_fonts();

    // --- All ---
    /** @brief 清空所有类型的资源 */
    void clear_all();

private:
    std::unordered_map<entt::id_type, std::unique_ptr<sf::Texture>> textures_;
    std::unordered_map<entt::id_type, std::unique_ptr<sf::SoundBuffer>> sounds_;
    std::unordered_map<entt::id_type, std::unique_ptr<sf::Music>> musics_;
    std::unordered_map<entt::id_type, std::unique_ptr<sf::Font>> fonts_;
};

} // namespace engine::resource