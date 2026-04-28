#include "engine/system/animation_system.hpp"
#include "engine/component/animation_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/utils/events.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace engine::system {

AnimationSystem::AnimationSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry_{registry}
    , dispatcher_{dispatcher} {
    dispatcher_.sink<engine::utils::PlayAnimationEvent>().connect<&AnimationSystem::on_play_animation_event>(this);
}

AnimationSystem::~AnimationSystem() {
    dispatcher_.disconnect(this);
}

void AnimationSystem::update(sf::Time delta) {
    auto view = registry_.view<engine::component::AnimationComponent, engine::component::SpriteComponent>();
    for (auto&& [entity, anim_component, sprite_component] : view.each()) {
        // 如果动画不存在，则跳过
        auto it = anim_component.animations_.find(anim_component.current_animation_id_);
        if (it == anim_component.animations_.end()) {
            continue;
        }

        // 获取当前动画
        auto& current_animation = it->second;
        // 如果没有帧，则跳过
        if (current_animation.frames_.empty()) {
            continue;
        }

        // 更新当前播放时间 (推进计时器)
        anim_component.current_time_ += delta * anim_component.speed_;

        // 连续推进多帧，直到时间不够切帧为止
        while (anim_component.current_time_ >= current_animation.frames_[anim_component.current_frame_index_].duration_) {
            anim_component.current_time_ -= current_animation.frames_[anim_component.current_frame_index_].duration_;
            anim_component.current_frame_index_++;

            if (anim_component.current_frame_index_ >= current_animation.frames_.size()) {
                if (current_animation.loop_) {
                    anim_component.current_frame_index_ = 0;
                } else {
                    // 停在最后一帧
                    anim_component.current_frame_index_ = current_animation.frames_.size() - 1;
                    // 清除残留时间，避免影响下一段动画
                    anim_component.current_time_ = sf::Time::Zero;
                    dispatcher_.enqueue(engine::utils::AnimationFinishedEvent{entity, anim_component.current_animation_id_});
                    break;
                }
            }
        }

        // 更新 Sprite 显示区域
        sprite_component.sprite_.setTextureRect(current_animation.frames_[anim_component.current_frame_index_].src_rect_);
    }
}

void AnimationSystem::on_play_animation_event(const engine::utils::PlayAnimationEvent& event) {
    // 使用try_get方法来安全获取可能存在的组件。如果不存在则返回nullptr
    if (auto anim = registry_.try_get<engine::component::AnimationComponent>(event.entity_); anim) {
        anim->current_animation_id_ = event.animation_id_;      // 替换动画ID
        anim->current_frame_index_ = 0;
        anim->current_time_ = sf::Time::Zero;
        anim->animations_.at(event.animation_id_).loop_ = event.loop_;
    }
}
} // namespace engine::system