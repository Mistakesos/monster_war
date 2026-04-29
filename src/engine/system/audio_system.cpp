#include "engine/system/audio_system.hpp"
#include "engine/core/context.hpp"
#include "engine/component/audio_component.hpp"
#include "engine/audio/audio_player.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::system {

AudioSystem::~AudioSystem() {
    auto& dispatcher = context_.get_dispatcher();
    dispatcher.disconnect(this);
}

AudioSystem::AudioSystem(entt::registry& registry, engine::core::Context& context)
    : registry_{registry}
    , context_{context} {
    auto& dispatcher = context_.get_dispatcher();
    dispatcher.sink<engine::utils::PlaySoundEvent>().connect<&AudioSystem::on_play_sound_event>(this);
}

void AudioSystem::on_play_sound_event(const engine::utils::PlaySoundEvent& event) {
    // 如果没有传入目标实体，则直接播放全局音效
    if (event.entity_ == entt::null) {
        spdlog::info("播放全局音效: {}", event.sound_id_);
        context_.get_audio_player().play_sound(event.sound_id_);
    }
    // 如果有传入目标实体，且实体有音效组件
    else if (auto audio_component = registry_.try_get<engine::component::AudioComponent>(event.entity_); audio_component) {
        auto it = audio_component->sounds_.find(event.sound_id_);
        // 先尝试在目标实体的音效集合中查找
        if (it != audio_component->sounds_.end()) {
            spdlog::info("实体 ID: {} 中找到了音效: {}", entt::to_integral(event.entity_), it->second);
            context_.get_audio_player().play_sound(it->second);
        // 如果没找到，则播放全局音效
        } else {
            spdlog::info("实体 ID: {} 中没有找到音效: {}", entt::to_integral(event.entity_), event.sound_id_);
            context_.get_audio_player().play_sound(event.sound_id_);
        }
    }
    // 如果有传入目标实体，但实体没有音效组件，也尝试播放全局音效
    else {
        spdlog::info("实体 ID: {} 中没有音效组件，尝试播放全局音效: {}", entt::to_integral(event.entity_), event.sound_id_);
        context_.get_audio_player().play_sound(event.sound_id_);
    }
}

} // namespace engine::system