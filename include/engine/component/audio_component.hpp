#pragma once
#include "component.hpp"
#include <string>
#include <unordered_map>

namespace engine::audio {
    class AudioPlayer;
} // namespace engine::audio

namespace engine::render {
    class Camera;
} // namespace engine::render

namespace engine::component {
class TransformComponent;

/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
class AudioComponent final: public Component {
    friend class engine::object::GameObject;
public:
    AudioComponent(engine::object::GameObject* owner, engine::audio::AudioPlayer* audio_player, engine::render::Camera* camera);
    ~AudioComponent() override = default;

    // 禁止拷贝和移动
    AudioComponent(const AudioComponent&) = delete;
    AudioComponent& operator=(const AudioComponent&) = delete;
    AudioComponent(AudioComponent&&) = delete;
    AudioComponent& operator=(AudioComponent&&) = delete;

    /**
     * @brief 播放音效。
     * @param sound_path 音效文件的id (或路径)。
     * @param use_spatial 是否使用空间定位。
     */
    void play_sound(std::string_view sound_id, bool use_spatial = false);

    /**
     * @brief 添加音效到映射表。
     * @param sound_id 音效的标识符（针对本组件唯一即可）。
     * @param sound_path 音效文件的路径。
     */
    void add_sound(std::string_view sound_id, std::string_view sound_path);

private:
    // 核心循环方法
    void update(sf::Time, engine::core::Context&) override {}

    engine::audio::AudioPlayer* audio_player_obs_;                      ///< @brief 音频播放器的非拥有指针
    engine::render::Camera* camera_obs_;                                ///< @brief 相机的非拥有指针，用于音频空间定位
    engine::component::TransformComponent* transform_obs_ = nullptr;    ///< @brief 缓存变换组件

    std::unordered_map<std::string, std::string> sound_id_to_path_;     ///< @brief 音效id 到路径的映射表
};
} // namespace engine::component