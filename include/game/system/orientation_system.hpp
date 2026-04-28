#pragma once
#include <entt/entity/fwd.hpp>

namespace game::system {

/**
 * @brief 朝向系统，用于正确处理角色的朝向（面向左或者右）。
 */
class OrientationSystem {
public:
    void update(entt::registry& registry);

private:
    // 拆分逻辑的函数，在update中调用
    void update_has_target(entt::registry& registry); ///< @brief 处理有目标的角色
    void update_blocked(entt::registry& registry);   ///< @brief 处理被阻挡的敌方角色
    void update_moving(entt::registry& registry);    ///< @brief 处理移动中的敌方角色
};

}   // namespace game::system