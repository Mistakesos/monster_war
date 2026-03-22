#include "engine/system/animation_system.hpp"
#include "engine/component/animation_component.hpp"
#include "engine/component/sprite_component.hpp"
#include <entt/entity/registry.hpp>

namespace engine::system {

void AnimationSystem::update(entt::registry& registry, sf::Time delta) {
    auto view = registry.view<engine::component::AnimationComponent, engine::component::SpriteComponent>();
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

        // 获取当前帧
        const auto& current_frame = current_animation.frames_[anim_component.current_frame_index_];

        // 检查是否需要切换到下一帧
        while (anim_component.current_time_ >= current_frame.duration_) {
            anim_component.current_time_ -= current_frame.duration_;
            anim_component.current_frame_index_++;

            if (anim_component.current_frame_index_ >= current_animation.frames_.size()) {
                if (current_animation.loop_) {
                    anim_component.current_frame_index_ = 0;
                } else {
                    anim_component.current_frame_index_ = current_animation.frames_.size() - 1;
                    break; // 如果动画已结束，停止处理
                }
            }
        }
        // 更新 SpriteComponent 显示区域 （根据当前动画帧的显示区域信息）
        const auto& next_frame = current_frame;
        sprite_component.sprite_.setTextureRect(next_frame.src_rect_);
    }
}

} // namespace engine::system