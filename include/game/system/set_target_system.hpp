#pragma once
#include <entt/entity/fwd.hpp>

namespace game::system {

/**
 * @brief 设置目标系统，用于设置角色的攻击目标。
 */
class SetTargetSystem {
public:
    void update(entt::registry& registry);

private:
    // 拆分逻辑的函数，在update中调用
    void update_has_target(entt::registry& registry);         ///< @brief 处理有目标的角色
    void update_no_target_player(entt::registry& registry);    ///< @brief 处理没有目标的玩家攻击型角色
    void update_no_target_enemy(entt::registry& registry);     ///< @brief 处理没有目标的敌人角色
    void update_healer(entt::registry& registry);            ///< @brief 处理治疗者
};

}   // namespace game::system