#pragma once
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <SFML/System/Time.hpp>

namespace game::system {

/**
 * @brief 计时器系统，用于更新所有包含计时器的组件，
 * 并在满足条件时添加必要的标签，（如攻击冷却完成后，添加“可攻击”标签）。
 */
class TimerSystem {
public:
    TimerSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~TimerSystem() = default;

    void update(sf::Time delta);

private:
    // 拆分逻辑的函数，在update中调用
    void update_attack_timer(sf::Time delta);           ///< @brief 处理攻击计时器
    void update_skill_cooldown_timer(sf::Time delta);   ///< @brief 处理技能冷却计时器
    void update_skill_duration_timer(sf::Time delta);   ///< @brief 处理技能持续计时器
    // TODO: 处理其他计时器

    entt::registry& registry_;
    entt::dispatcher& dispatcher_;
};

}