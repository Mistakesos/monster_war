#pragma once
#include "engine/ui/ui_element.hpp"
#include "engine/core/context.hpp"
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <string_view>
#include <entt/core/hashed_string.hpp>

namespace engine::ui {
/**
 * @brief UILabel 类用于创建和管理用户界面中的文本标签
 * * UILabel 继承自 UIElement，提供了文本渲染功能。
 * 它可以设置文本内容、字体ID、字体大小和文本颜色。
 */
class UILabel final : public UIElement {
public:
    /**
     * @brief 构造一个UILabel
     * @param context 上下文
     * @param text 文本内容
     * @param font_path 字体路径 (用于生成哈希ID和加载临时字体)
     * @param font_size 字体大小
     * @param text_color 文本颜色
     */
    UILabel(engine::core::Context& context,
            std::string_view text,
            std::string_view font_path,
            int font_size = 16,
            sf::Color text_color = sf::Color::White,
            sf::Vector2f position = {0.0f, 0.0f});

    // --- 核心方法 ---
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    std::string_view get_text() const { return text_; }
    std::string_view get_font_path() const { return font_path_; }
    entt::id_type get_font_id() const { return font_id_; }
    int get_font_size() const { return font_size_; }
    const sf::Color& get_text_color() const { return text_color_; }

    void set_text(std::string_view text);                      ///< @brief 设置文本内容, 同时更新尺寸
    void set_font_path(std::string_view font_path);            ///< @brief 设置字体路径, 同时更新ID和尺寸
    void set_font_size(int font_size);                         ///< @brief 设置字体大小, 同时更新尺寸
    void set_text_color(sf::Color text_color);                 ///< @brief 设置字体颜色

private:
    void update_size();  ///< @brief 统一更新文本尺寸的内部方法

    engine::core::Context& context_;   
    
    std::string text_;                          ///< @brief 文本内容    
    std::string font_path_;                     ///< @brief 字体路径
    entt::id_type font_id_;                     ///< @brief 字体哈希ID
    int font_size_;                             ///< @brief 字体大小   
    sf::Color text_color_ = sf::Color::White;   ///< @brief 字体颜色
};
} // namespace engine::ui