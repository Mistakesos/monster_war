#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace engine::component {
/**
 * @brief 管理 GameObject 的视觉表示，通过持有一个 Sprite 对象。
 *
 * 协调 Sprite 数据和渲染逻辑，并与 TransformComponent 交互。
 */
struct SpriteComponent {
    SpriteComponent(const sf::Texture& texture)
        : sprite_{texture} {
    }
    SpriteComponent(sf::Sprite& sprite)
        : sprite_{sprite} {
    }

    sf::Sprite sprite_;                 ///< @brief 内部储存的精灵
    bool is_visible_{true};             ///< @brief 是否隐藏（不渲染）
};
} // namespace engine::component