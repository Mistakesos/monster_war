#pragma once
#include <entt/entity/entity.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace game::component {

/// @brief 投射物组件, 附加在投射物实体上
struct ProjectileComponent {
    entt::entity target_{entt::null};       ///< @brief 目标实体
    float damage_{};                        ///< @brief 伤害
    sf::Vector2f start_position_{};         ///< @brief 起始位置
    sf::Vector2f target_position_{};        ///< @brief 目标位置
    sf::Vector2f previous_position_{};      ///< @brief 上一帧位置（用于计算朝向）
    float arc_height_{};                    ///< @brief 弧度高度(即正弦函数振幅)
    sf::Time total_flight_time_{};          ///< @brief 总飞行时间
    sf::Time current_flight_time_{};        ///< @brief 当前飞行时间
};

/// @brief 投射物ID组件, 附加在远程攻击角色上
struct ProjectileIDComponent {
    entt::id_type id_{entt::null};          ///< @brief 投射物ID
};

}