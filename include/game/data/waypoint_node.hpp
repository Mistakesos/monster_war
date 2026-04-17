#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace game::data {

/**
 * @brief 路径节点数据结构。
 * @note 包含节点ID、坐标和指向下一个节点的ID列表。
 */
struct WaypointNode {
    int id_ = 0;
    sf::Vector2f position_ = {0.f, 0.f};
    std::vector<int> next_node_ids_ = {0};    // 指向下一个节点的ID列表
};

} // namespace game::data