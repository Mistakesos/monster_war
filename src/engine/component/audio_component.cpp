#include "engine/component/audio_component.hpp"
#include "engine/component/transform_component.hpp"
#include "engine/object/game_object.hpp"
#include "engine/audio/audio_player.hpp"
#include "engine/render/camera.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {

AudioComponent::AudioComponent(engine::object::GameObject* owner, engine::audio::AudioPlayer* audio_player, engine::render::Camera* camera)
    : Component{owner}
    , audio_player_obs_{audio_player}
    , camera_obs_{camera} {
    if (!audio_player_obs_ || !camera_obs_) {
        spdlog::error("AudioComponent 初始化失败: 音频播放器或相机为空");
    }
    transform_obs_ = owner_->get_component<TransformComponent>();
    if (!transform_obs_) {
        spdlog::warn("AudioComponent 所在的 GameObject 上没有 TransformComponent！，无法进行空间定位");
    }
}

void AudioComponent::play_sound(std::string_view sound_id, bool use_spatial)
{
    // 如果 sound_id 是音效 ID，则在查找在map中查找对应的路径； 没找到的话则把 sound_id 当作路径直接使用
    auto sound_path = sound_id_to_path_.find(std::string(sound_id)) != sound_id_to_path_.end() ? sound_id_to_path_[std::string(sound_id)] : sound_id;

    if (use_spatial && transform_obs_) {    // 使用空间定位
        // TODO: (SDL_Mixer 不支持空间定位，未来更换音频库时可以方便地实现)
                // 这里给一个简单的功能：150像素范围内播放，否则不播放
        auto camera_center = camera_obs_->get_world_view_center(); // 相机中心
        auto object_pos = transform_obs_->get_position();
        float distance = (camera_center - object_pos).length();
        if (distance > 150.f) {
            spdlog::debug("AudioComponent::playSound: 音效 '{}' 超出范围，不播放。", sound_id);
            return; // 超出范围，不播放
        }
        audio_player_obs_->play_sound(sound_path);
    } else {    // 不使用空间定位
        audio_player_obs_->play_sound(sound_path);
    }
}

void AudioComponent::add_sound(std::string_view sound_id, std::string_view sound_path)
{
    if (sound_id_to_path_.find(std::string(sound_id)) != sound_id_to_path_.end()) {
        spdlog::warn("AudioComponent::add_sound: 音效 ID '{}' 已存在，覆盖旧路径。", sound_id);
    }
    sound_id_to_path_[std::string(sound_id)] = sound_path;
    spdlog::debug("AudioComponent::add_sound: 添加音效 ID '{}' 路径 '{}'", sound_id, sound_path);
}
} // namespace engine::component