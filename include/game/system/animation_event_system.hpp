#pragma once
#include "engine/utils/events.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 动画事件系统，用于处理各种动画事件
 */
class AnimationEventSystem {
public:
    AnimationEventSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~AnimationEventSystem();

private:
    // 事件回调函数
    void on_animation_event(const engine::utils::AnimationEvent& event);

    // 拆分不同的事件类型
    void handle_hit_event(const engine::utils::AnimationEvent& event);    ///< @brief 命中事件
    void handle_emit_event(const engine::utils::AnimationEvent& event);   ///< @brief 发射事件
    
    entt::registry& registry_;
    entt::dispatcher& dispatcher_;
};

} // namespace game::system