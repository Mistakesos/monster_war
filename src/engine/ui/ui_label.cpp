#include "engine/ui/ui_label.hpp"
#include "engine/core/context.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <spdlog/spdlog.h>

namespace engine::ui {
UILabel::UILabel(engine::render::Renderer& render
               , std::string_view text
               , std::string_view font_id
               , int font_size
               , sf::Color text_color
               , sf::Vector2f position)
    : UIElement{std::move(position)}
    , render_{render}
    , text_{text}
    , font_id_{font_id}
    , font_size_{font_size}
    , text_color_{std::move(text_color)} {
    // 获取文本渲染尺寸
    set_text(text_);    // 为了调整一次尺寸
    spdlog::trace("UILabel 构造完成");
}

void UILabel::render(engine::core::Context& context) {
    if (!visible_ || text_.empty()) return;
    auto& camera  = context.get_camera();

    render_.draw_ui_text(camera, text_, font_id_, font_size_, get_screen_position(), text_color_);

    // 渲染子元素（调用基类方法）
    UIElement::render(context);
}

void UILabel::set_text(std::string_view text) {
    text_ = text;
    sf::Font font_temp(font_id_);
    sf::Text text_temp(font_temp, sf::String::fromUtf8(text_.begin(), text_.end()));
    text_temp.setCharacterSize(font_size_);
    size_ = text_temp.getGlobalBounds().size;
}

void UILabel::set_font_id(std::string_view font_id) {
    font_id_ = font_id;
    sf::Font font_temp(font_id_);
    sf::Text text_temp(font_temp, sf::String::fromUtf8(text_.begin(), text_.end()));
    text_temp.setCharacterSize(font_size_);
    size_ = text_temp.getGlobalBounds().size;
}

void UILabel::set_font_size(int font_size) {
    font_size_ = font_size;
    sf::Font font_temp(font_id_);
    sf::Text text_temp(font_temp, sf::String::fromUtf8(text_.begin(), text_.end()));
    text_temp.setCharacterSize(font_size_);
    size_ = text_temp.getGlobalBounds().size;
}

void UILabel::set_text_color(sf::Color text_color) {
    text_color_ = std::move(text_color);
    /* 颜色变化不影响尺寸 */
}
} // namespace engine::ui