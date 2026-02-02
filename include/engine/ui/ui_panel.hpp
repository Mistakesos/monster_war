#pragma once
#include "engine/ui/ui_element.hpp"
#include <SFML/Graphics/Color.hpp>
#include <optional>

namespace engine::ui {
/**
 * @brief 用于分组其他UI元素的容器UI元素
 *
 * Panel通常用于布局和组织。
 * 可以选择是否绘制背景色(纯色)。
 */
class UIPanel final : public UIElement {
public:
    /**
     * @brief 构造一个Panel
     *
     * @param position Panel的局部位置  
     * @param size Panel的大小
     * @param background_color 背景色
     */
    explicit UIPanel(sf::Vector2f position = {0.0f, 0.0f},
                     sf::Vector2f size = {0.0f, 0.0f},
                     std::optional<sf::Color> background_color = std::nullopt);

    void set_background_color(const std::optional<sf::Color>& background_color) { background_color_ = background_color; }
    const std::optional<sf::Color>& get_background_color() const { return background_color_; }

    void render(engine::core::Context& context) override;

private:
    std::optional<sf::Color> background_color_;    ///< @brief 可选背景色
};
} // namespace engine::ui