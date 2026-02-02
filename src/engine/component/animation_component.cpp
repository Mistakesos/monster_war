#include "engine/component/animation_component.hpp"
#include "engine/component/sprite_component.hpp"
#include "engine/object/game_object.hpp"
#include "engine/render/animation.hpp"
#include <spdlog/spdlog.h>

namespace engine::component{
AnimationComponent::AnimationComponent(engine::object::GameObject* owner)
    : Component{owner} {
    sprite_component_obs_ = owner_->get_component<SpriteComponent>();
    if (!sprite_component_obs_) {
        spdlog::error("GameObject '{}' 的 AnimationComponent 需要 SpriteComponent，但未找到。", owner_->get_name());
        return;
    }
}

AnimationComponent::~AnimationComponent() = default;

void AnimationComponent::add_animation(std::unique_ptr<engine::render::Animation> animation) {
    if (!animation) return;
    std::string_view name = animation->get_name();    // 获取名称
    animations_[std::string(name)] = std::move(animation);
    spdlog::debug("已将动画 '{}' 添加到 GameObject '{}'", name, owner_->get_name());
}

void AnimationComponent::play_animation(std::string_view name) {
    auto it = animations_.find(std::string(name));
    if (it == animations_.end() || !it->second) {
        spdlog::warn("未找到 GameObject '{}' 的动画 '{}'", name, owner_->get_name());
        return;
    }

    // 如果已经在播放相同的动画，不重新开始（注释这一段则重新开始播放）
    if (current_animation_obs_ == it->second.get() && is_playing_) {
        return;
    }

    current_animation_obs_ = it->second.get();
    animation_timer_ = sf::Time::Zero;
    is_playing_ = true;

    // 立即将精灵更新到第一帧
    if (sprite_component_obs_ && !current_animation_obs_->is_empty()) {
        const auto& first_frame = current_animation_obs_->get_frame(sf::Time::Zero);
        sf::Sprite& sprite = sprite_component_obs_->get_sprite();
        sprite.setTextureRect(first_frame.source_rect);
        spdlog::debug("GameObject '{}' 播放动画 '{}'", owner_->get_name(), name);
    }
}

std::string_view AnimationComponent::get_current_animation_name() const {
    if (current_animation_obs_) {
        return current_animation_obs_->get_name();
    }
    return std::string_view();      // 返回一个空的string_view
}

bool AnimationComponent::is_animation_finished() const {
    // 如果没有当前动画(说明从未调用过playAnimation)，或者当前动画是循环的，则返回 false
    if (!current_animation_obs_ || current_animation_obs_->is_looping()) {
        return false;
    }
    return animation_timer_ >= current_animation_obs_->get_total_duration();
}

void AnimationComponent::update(sf::Time delta, engine::core::Context&) {
    // 如果没有正在播放的动画，或者没有当前动画，或者没有精灵组件，或者当前动画没有帧，则直接返回
    if (!is_playing_ || !current_animation_obs_ || !sprite_component_obs_ || current_animation_obs_->is_empty()) {
        spdlog::trace("AnimationComponent 更新时没有正在播放的动画或精灵组件为空。");
        return;
    }

    // 推进计时器
    animation_timer_ += delta;

    // 根据时间获取当前帧
    const auto& current_frame = current_animation_obs_->get_frame(animation_timer_);

    // 更新精灵组件的源矩形 (使用 SpriteComponent 的新方法)
    sf::Sprite& sprite = sprite_component_obs_->get_sprite();
    sprite.setTextureRect(current_frame.source_rect);

    // 检查非循环动画是否已结束
    if (!current_animation_obs_->is_looping() && animation_timer_ >= current_animation_obs_->get_total_duration()) {
        is_playing_ = false;
        animation_timer_ = current_animation_obs_->get_total_duration(); // 将时间限制在结束点
        if (is_one_shot_removal_) {     // 如果 is_one_shot_removal_ 为 true，则删除整个 GameObject
            owner_->set_need_remove(true);
        }
    }
}
} // namespace engine::component