#pragma once
#include "engine/utils/events.hpp"
#include <entt/entity/fwd.hpp>

namespace engine::core {
class Context;
} // namespace engine::core

namespace engine::system {

/**
 * @brief 音频系统，负责处理播放音频事件。
 */
class AudioSystem{
public:
    AudioSystem(entt::registry& registry, engine::core::Context& context);
    ~AudioSystem();

private:
    void on_play_sound_event(const engine::utils::PlaySoundEvent& event);

    entt::registry& registry_;
    engine::core::Context& context_;
};
} // namespace engine::system