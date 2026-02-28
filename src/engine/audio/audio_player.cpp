#include "engine/audio/audio_player.hpp"
#include "engine/resource/resource_manager.hpp"
#include <entt/core/hashed_string.hpp>
#include <entt/entity/entity.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::audio {

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* rm)
    : resource_manager_obs_{rm} {
    if (!resource_manager_obs_) throw std::runtime_error("AudioPlayer: ResourceManager 为空");
    spdlog::info("AudioPlayer 初始化成功");
}

AudioPlayer::~AudioPlayer() {
    stop_music();
}

// ========================= 音效逻辑 =========================

sf::Sound* AudioPlayer::play_sound(entt::id_type sound_id, bool loop, std::optional<float> volume) {
    // 1. 清理已停止的音效
    std::erase_if(active_sounds_, [](const auto& s) { 
        return s->getStatus() == sf::SoundSource::Status::Stopped; 
    });

    // 2. 获取 Buffer
    auto* buffer = resource_manager_obs_->get_sound(sound_id);
    if (!buffer) return nullptr;

    // 3. 播放
    auto& sound = active_sounds_.emplace_back(std::make_unique<sf::Sound>(*buffer));
    sound->setVolume(volume.value_or(sound_volume_));
    sound->setLooping(loop);
    sound->play();

    return sound.get();
}

sf::Sound* AudioPlayer::play_sound(entt::hashed_string hs, bool loop, std::optional<float> volume) {
    // 确保资源已加载，然后复用 ID 版本的逻辑
    resource_manager_obs_->get_sound(hs);
    return play_sound(hs.value(), loop, volume);
}

// ========================= 音乐逻辑 =========================

bool AudioPlayer::play_music(entt::id_type music_id, bool loop) {
    if (music_id == current_music_id_) {
        if (auto* m = resource_manager_obs_->get_music(music_id)) {
            if (m->getStatus() == sf::Music::Status::Playing) return true;
        }
    }

    stop_music();

    if (auto* music = resource_manager_obs_->get_music(music_id)) {
        music->setVolume(music_volume_);
        music->setLooping(loop);
        music->play();
        current_music_id_ = music_id;
        return true;
    }
    return false;
}

bool AudioPlayer::play_music(entt::hashed_string hs, bool loop) {
    resource_manager_obs_->get_music(hs); // 确保资源到位
    return play_music(hs.value(), loop);
}

void AudioPlayer::stop_music() {
    if (current_music_id_ == entt::null) return;
    if (auto* m = resource_manager_obs_->get_music(current_music_id_)) m->stop();
    current_music_id_ = entt::null;
}

void AudioPlayer::pause_music() {
    if (auto* m = resource_manager_obs_->get_music(current_music_id_)) m->pause();
}

void AudioPlayer::resume_music() {
    if (auto* m = resource_manager_obs_->get_music(current_music_id_)) m->play();
}

// ========================= 音量控制 =========================

void AudioPlayer::set_sound_volume(float volume) {
    sound_volume_ = std::clamp(volume, 0.0f, 100.0f);
    for (auto& s : active_sounds_) s->setVolume(sound_volume_);
}

void AudioPlayer::set_music_volume(float volume) {
    music_volume_ = std::clamp(volume, 0.0f, 100.0f);
    if (auto* m = resource_manager_obs_->get_music(current_music_id_)) m->setVolume(music_volume_);
}

float AudioPlayer::get_sound_volume() const { return sound_volume_; }
float AudioPlayer::get_music_volume() const { return music_volume_; }

} // namespace engine::audio