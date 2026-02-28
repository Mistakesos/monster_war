#pragma once
#include "engine/ui/ui_element.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <string_view>
#include <optional>
#include <entt/entity/entity.hpp>
#include <entt/core/hashed_string.hpp>

// 前置声明 Context，供构造函数使用
namespace engine::core {
    class Context;
}

namespace engine::ui {

/**
 * @brief 一个用于显示纹理或部分纹理的UI元素。
 *
 * 继承自UIElement并添加了渲染图像的功能。
 */
class UIImage final : public UIElement {
public:
    /**
     * @brief 构造一个UIImage对象。（通过纹理路径构造）
     * @note 需传入 Context 以便在初始化列表中直接解析出纹理
     */
    UIImage(engine::core::Context& context
          , std::string_view texture_path
          , sf::Vector2f position = sf::Vector2f{0.f, 0.f}
          , sf::Vector2f size = sf::Vector2f{0.f, 0.f}
          , const std::optional<sf::IntRect>& texture_rect = std::nullopt
          , bool is_flipped = false);

    /**
     * @brief 构造一个UIImage对象。（通过哈希ID构造）
     */
    UIImage(engine::core::Context& context
          , entt::id_type texture_id
          , sf::Vector2f position = sf::Vector2f{0.f, 0.f}
          , sf::Vector2f size = sf::Vector2f{0.f, 0.f}
          , const std::optional<sf::IntRect>& texture_rect = std::nullopt
          , bool is_flipped = false);

    /**
     * @brief 兼容原来的构造方式，直接传入纹理引用。（对应教程中的 Sprite 直接构造）
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

    // 顺便加上获取ID的方法
    entt::id_type get_texture_id() const { return texture_id_; }

protected:
    sf::Sprite sprite_;
    entt::id_type texture_id_{entt::null}; // 记录当前纹理的哈希ID
};

} // namespace engine::ui