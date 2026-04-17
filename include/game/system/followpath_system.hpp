#pragma once
#include "game/data/waypoint_node.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <SFML/System/Time.hpp>
#include <unordered_map>

namespace game::system {
/**
 * @brief 路径跟随系统。
 * 根据路径节点更新敌人实体的速度和目标节点。
 * @warning ！不要使用 delta 来实际处理速度，或者处理 delta 相关逻辑，这里用于防止抖动的运算
 */
class FollowPathSystem {
public:
    void update(entt::registry& registry, 
                entt::dispatcher& dispatcher, 
                std::unordered_map<int, game::data::WaypointNode>& waypoint_nodes,
                sf::Time delta);
};

} // namespace game::system