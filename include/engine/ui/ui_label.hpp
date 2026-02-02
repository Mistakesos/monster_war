#pragma once
#include "engine/ui/ui_element.hpp"
#include "engine/render/render.hpp"
#include <SFML/Graphics/Color.hpp>

namespace engine::ui {
/**
 * @brief UILabel 类用于创建和管理用户界面中的文本标签
 * 
 * UILabel 继承自 UIElement，提供了文本渲染功能。
 * 它可以设置文本内容、字体ID、字体大小和文本颜色。
 * 
 * @note 需要一个文本渲染器来获取和更新文本尺寸。
 */
class UILabel final : public UIElement {
public:
    /**
     * @brief 构造一个UILabel
     * 
     * @param render 渲染器
     * @param text 文本内容
     * @param font_id 字体ID
     * @param font_size 字体大小
     * @param text_color 文本颜色
     */
    UILabel(engine::render::Renderer& render,
            std::string_view text,
            std::string_view font_id,
            int font_size = 16,
            sf::Color color = sf::Color::White,
            sf::Vector2f position = {0.0f, 0.0f});

    // --- 核心方法 ---
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    std::string_view get_text() const { return text_; }
    std::string_view get_font_id() const { return font_id_; }
    int get_font_size() const { return font_size_; }
    const sf::Color& get_text_color() const { return text_color_; }

    void set_text(std::string_view text);                      ///< @brief 设置文本内容, 同时更新尺寸
    void set_font_id(std::string_view font_id);                ///< @brief 设置字体ID, 同时更新尺寸
    void set_font_size(int font_size);                         ///< @brief 设置字体大小, 同时更新尺寸
    void set_text_color(sf::Color text_color);                 ///< @brief 设置字体颜色

private:
    engine::render::Renderer& render_;   ///< @brief 需要文本渲染器，用于获取/更新文本尺寸
    
    std::string text_;                          ///< @brief 文本内容    
    std::string font_id_;                       ///< @brief 字体ID
    int font_size_;                             ///< @brief 字体大小   
    sf::Color text_color_ = sf::Color::White;   ///< @brief 字体颜色
    /* 可添加其他内容，例如边框、底色 */
};
} // namespace engine::ui