#include "engine/audio/audio_player.hpp"
#include "engine/resource/resource_manager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace engine::audio {

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* rm)
    : resource_manager_obs_{rm} {
    if (!resource_manager_obs_) {
        throw std::runtime_error("AudioPlayer: ResourceManager 指针为空");
    }
    spdlog::info("AudioPlayer 初始化成功");
}

AudioPlayer::~AudioPlayer() {
    stop_music();
    active_sounds_.clear();
}

// ========================= 音效 =========================
sf::Sound* AudioPlayer::play_sound(std::string_view sound_path, bool loop, std::optional<float> volume) {
    // 清理播放结束的音效
    std::erase_if(active_sounds_, [](const auto& s) {
        return s->getStatus() == sf::SoundSource::Status::Stopped;
    });

    auto* buffer = resource_manager_obs_->get_sound(sound_path);
    if (!buffer) {
        spdlog::error("AudioPlayer: 无法加载音效 '{}'", sound_path);
        return nullptr;
    }

    auto sound = std::make_unique<sf::Sound>(*buffer);
    if (volume.has_value())
    {
        sound->setVolume(std::clamp(volume.value(), 0.f, 100.f));
    } else {
        sound->setVolume(sound_volume_);
    }
    sound->setLooping(loop);
    sound->play();

    active_sounds_.push_back(std::move(sound));

    spdlog::trace("AudioPlayer: 播放音效 '{}', 音量: {}, 循环: {}", sound_path, volume.has_value() ? volume.value() : sound_volume_, loop);
    return active_sounds_.back().get();
}

void AudioPlayer::set_sound_volume(float volume) {
    sound_volume_ = std::clamp(volume, 0.f, 100.f);
    for (auto& sound : active_sounds_) {
        sound->setVolume(sound_volume_);
    }
    spdlog::trace("AudioPlayer: 全局音效音量设为 {:.1f}", sound_volume_);
}

float AudioPlayer::get_sound_volume() const {
    if (!active_sounds_.empty()) {
        return active_sounds_.front()->getVolume();
    }
    return sound_volume_;
}

// ========================= 音乐 =========================
bool AudioPlayer::play_music(std::string_view music_path, bool loop) {
    // 防止重复播放同一首
    if (current_music_path_ == music_path) {
        auto* music = resource_manager_obs_->get_music(music_path);
        if (music && music->getStatus() == sf::Music::Status::Playing) {
            return true;
        }
    }

    // 先停止旧的（如果有）
    stop_music();

    auto* music = resource_manager_obs_->get_music(music_path);
    if (!music) {
        spdlog::error("AudioPlayer: 无法加载音乐 '{}'", music_path);
        return false;
    }

    music->setVolume(music_volume_);
    music->setLooping(loop);
    music->play();
    current_music_path_ = music_path;

    spdlog::info("AudioPlayer: 开始播放音乐 '{}', 循环: {}", music_path, loop);
    return true;
}

void AudioPlayer::stop_music() {
    if (current_music_path_.empty()) return;

    if (auto* music = resource_manager_obs_->get_music(current_music_path_)) {
        music->stop();
    }
    current_music_path_.clear();
    spdlog::trace("AudioPlayer: 停止背景音乐");
}

void AudioPlayer::pause_music() {
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_obs_->get_music(current_music_path_)) {
        music->pause();
    }
}

void AudioPlayer::resume_music() {
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_obs_->get_music(current_music_path_)) {
        music->play();
    }
}

void AudioPlayer::set_music_volume(float volume) {
    music_volume_ = std::clamp(volume, 0.f, 100.f);
    if (current_music_path_.empty()) return;
    if (auto* music = resource_manager_obs_->get_music(current_music_path_)) {
        music->setVolume(music_volume_);
    }
    spdlog::trace("AudioPlayer: 音乐音量设为 {}", music_volume_);
}

float AudioPlayer::get_music_volume() const {
    if (auto* music = resource_manager_obs_->get_music(current_music_path_)) {
        return music->getVolume();
    }
    return music_volume_;
}
} // namespace engine::audio