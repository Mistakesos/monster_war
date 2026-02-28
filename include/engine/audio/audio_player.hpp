#pragma once
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <SFML/Audio.hpp>
#include "entt/entity/fwd.hpp"

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::audio {
/**
 * @brief 基于 SFML 的音频播放器
 * 音量范围：0-100（SFML 原生，直观）
 */
class AudioPlayer final {
public:
    explicit AudioPlayer(engine::resource::ResourceManager* rm);
    ~AudioPlayer();

    // 删除拷贝和移动（单例风格）
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    AudioPlayer(AudioPlayer&&) = delete;
    AudioPlayer& operator=(AudioPlayer&&) = delete;

    // --- 音效控制 ---
    /**
     * @brief 播放音效
     * @note 必须确保 ResourceManager 加载了音效。
     * @param sound_id 音效id
     * @param volume 音量 0-100，默认 100
     * @param loop 是否循环，默认 false
     * @return 返回 sf::Sound* 可用于后续控制（pause/stop/setVolume），失败返回 nullptr
     */
    sf::Sound* play_sound(entt::id_type sound_id, bool loop = false, std::optional<float> volume = std::nullopt);

    /**
     * @brief 播放音效
     * @note 如果尚未缓存，则通过 ResourceManager 加载音效。
     * @param hashed_path 音效文件路径的哈希值。
     * @param volume 音量 0-100，默认 100
     * @param loop 是否循环，默认 false
     * @return 返回 sf::Sound* 可用于后续控制（pause/stop/setVolume），失败返回 nullptr
     */
    sf::Sound* play_sound(entt::hashed_string hashed_path, bool loop = false, std::optional<float> volume = std::nullopt);
    
    /**
     * @brief 设置所有音效的全局音量
     */
    void set_sound_volume(float volume);  // 0-100

    /**
     * @brief 获取当前音效音量（取第一个活跃音效为准）
     */
    float get_sound_volume() const;

    // --- 音乐控制 ---
    /**
     * @brief 播放背景音乐（自动停止上一首）
     * @param music_id 音乐id
     * @param loop 是否循环，默认 true
     * @return 成功返回 true
     */
    bool play_music(entt::id_type music_id, bool loop = true);

    /**
     * @brief 播放背景音乐（自动停止上一首）
     * @param hashed_path 音乐文件路径的哈希值
     * @param loop 是否循环，默认 true
     * @return 成功返回 true
     */
    bool play_music(entt::hashed_string hashed_path, bool loop = true);

    void stop_music();
    void pause_music();
    void resume_music();

    void set_music_volume(float volume);  // 0-100
    float get_music_volume() const;

private:
    engine::resource::ResourceManager* resource_manager_obs_;

    // 正在播放的音效实例（用于控制音量、暂停等）
    std::vector<std::unique_ptr<sf::Sound>> active_sounds_;

    // 当前背景音乐ID（防止重复播放）
    entt::id_type current_music_id_;   ///< @brief 当前正在播放的音乐ID，用于避免重复播放同一音乐。

    float music_volume_ = 100.f;
    float sound_volume_ = 100.f;
};
} // namespace engine::audio