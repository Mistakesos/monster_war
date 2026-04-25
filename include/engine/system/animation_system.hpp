#pragma once
#include "engine/utils/events.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <SFML/System/Time.hpp>

namespace engine::system {

/**
 * @brief 动画系统
 * 
 * 负责更新实体的动画组件，并同步到精灵组件。
 */
class AnimationSystem {
public:
    AnimationSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~AnimationSystem();

    /**
     * @brief 更新所有拥有动画和精灵组件的实体
     * @param registry entt注册表
     * @param dt 增量时间
     */
    void update(sf::Time delta);

private:
    void on_play_animation_event(const engine::utils::PlayAnimationEvent& event);  ///< @brief 播放动画事件处理函数

    // 将依赖保存为成员变量，方便回调函数使用
    entt::registry& registry_;
    entt::dispatcher& dispatcher_;
};

} // namespace engine::system