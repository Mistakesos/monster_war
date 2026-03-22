#pragma once
#include <SFML/System/Vector2.hpp>

namespace engine::component {

/**
 * @brief 速度组件
 */
struct VelocityComponent
{
    sf::Vector2f velocity_{};   ///< @brief 速度
};

} // namespace engine::component