#pragma once
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 攻击启动系统，用于启动角色的攻击动作。
 */
class AttackStarterSystem {
public:
    void update(entt::registry& registry, entt::dispatcher& dispatcher);

private:
    // 拆分逻辑的函数，在update中调用
    void update_enemy_blocked(entt::registry& registry, entt::dispatcher& dispatcher); ///< @brief 处理被阻挡敌人
    void update_enemy_ranged(entt::registry& registry, entt::dispatcher& dispatcher);  ///< @brief 处理敌人远程
    void update_player(entt::registry& registry, entt::dispatcher& dispatcher);        ///< @brief 处理玩家
};

} // namespace game::system