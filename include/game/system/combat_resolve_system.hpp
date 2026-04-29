#pragma once
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include "game/defs/events.hpp"

namespace game::system {

/**
 * @brief 战斗结算系统，用于处理战斗结算
 * 
 * 根据接受到的事件（攻击/治疗），执行相应的结算操作。
 */
class CombatResolveSystem {
public:
    CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~CombatResolveSystem();

private:
    // 事件回调函数
    void on_attack_event(const game::defs::AttackEvent& event);
    void on_heal_event(const game::defs::HealEvent& event);

    /**
     * @brief 计算最终伤害（公式可修改）
     * 当前计算公式：攻击力 - 防御力，最小伤害为攻击力的10%
     */
    float calculate_effective_damage(float attacker_atk, float target_def);
    
    entt::registry& registry_;
    entt::dispatcher& dispatcher_;
};

} // namespace game::system