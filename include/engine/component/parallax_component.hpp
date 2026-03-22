#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <string>

namespace engine::component {
/**
 * @brief 视差组件，包含滚动速度因子、是否重复和是否可见。（需和Sprite配合使用）
 */
struct ParallaxComponent {
    ParallaxComponent(sf::Vector2f scroll_factor,
                      sf::Vector2<bool> repeat = {true, false},
                      bool is_visible = true)
        : scroll_factor_{std::move(scroll_factor)}
        , repeat_{repeat}
        , is_visible_{is_visible} {
    }

    sf::Vector2f scroll_factor_;        ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
    sf::Vector2<bool> repeat_;          ///< @brief 是否沿着X和Y轴周期性重复
    bool is_visible_{true};             ///< @brief 是否隐藏（不渲染）
};
} // namespace engine::component