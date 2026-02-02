#pragma once
#include "engine/ui/ui_element.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <optional>

namespace engine::ui {
/**
 * @brief 一个用于显示纹理或部分纹理的UI元素。
 *
 * 继承自UIElement并添加了渲染图像的功能。
 */
class UIImage final : public UIElement {
public:
    /**
     * @brief 构造一个UIImage对象。
     *
     * @param texture 要显示的纹理。
     * @param position 图像的局部位置。
     * @param size 图像元素的大小。（如果为{0,0}，则使用纹理的原始尺寸）
     * @param texture_rect 可选：要绘制的纹理部分。（如果为空，则使用纹理的整个区域）
     * @param is_flipped 可选：精灵是否应该水平翻转。
     */
    UIImage(const sf::Texture& texture
          , sf::Vector2f position = sf::Vector2f{0.f, 0.f}
          , sf::Vector2f size = sf::Vector2f{0.f, 0.f}
          , const std::optional<sf::IntRect>& texture_rect = std::nullopt
          , bool is_flipped = false);

    ~UIImage() override = default;

    // --- 核心方法 ---
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    const sf::Sprite& get_sprite() const { return sprite_; }
    void set_sprite(const sf::Sprite& sprite) { sprite_ = sprite; }

    bool is_flipped() const { return sprite_.getScale() != sf::Vector2f{1.f, 1.f}; }
    void set_flipped(bool flipped);

protected:
    sf::Sprite sprite_;
};
} // namespace engine::ui