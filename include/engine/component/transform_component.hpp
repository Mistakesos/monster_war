#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>

namespace engine::component {
/**
 * @class 变换组件，包含位置、缩放和旋转。
 */
struct TransformComponent {
    /**
     * @brief 构造函数
     * @param position 位置
     * @param scale 缩放
     * @param angle 旋转角度（兼容角度和弧度）
     */
    explicit TransformComponent(sf::Vector2f position = {0.f, 0.f},
                                sf::Vector2f scale = {1.f, 1.f},
                                sf::Angle angle = sf::degrees(0.f),
                                sf::Vector2f origin = {0.f, 0.f})
        : position_{std::move(position)}
        , scale_{std::move(scale)}
        , angle_{std::move(angle)}
        , origin_{std::move(origin)} {}
  
    sf::Vector2f position_ = {0.f, 0.f};        ///< @brief 位置
    sf::Vector2f scale_ = {1.f, 1.f};           ///< @brief 缩放
    sf::Angle angle_ = sf::degrees(0.f);        ///< @brief 角度制，单位：度（约定，实际上也支持弧度）
    sf::Vector2f origin_ = {0.f, 0.f};          ///< @brief 原点
};
} // namespace engine::component