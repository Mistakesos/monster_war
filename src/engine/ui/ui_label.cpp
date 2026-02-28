#include "engine/ui/ui_label.hpp"
#include "engine/render/render.hpp"
#include "engine/resource/resource_manager.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <spdlog/spdlog.h>

namespace engine::ui {
UILabel::UILabel(engine::core::Context& context
               , std::string_view text
               , std::string_view font_path
               , int font_size
               , sf::Color text_color
               , sf::Vector2f position)
    : UIElement{std::move(position)}
    , context_{context}
    , text_{text}
    , font_path_{font_path}
    // 关键：在初始化列表中直接生成哈希ID
    , font_id_{entt::hashed_string(font_path.data())}
    , font_size_{font_size}
    , text_color_{std::move(text_color)} {
    
    update_size(); 
    spdlog::trace("UILabel 构造完成");
}

void UILabel::render(engine::core::Context& context) {
    if (!visible_ || text_.empty()) return;
    auto& camera  = context.get_camera();

    // 注意：请确保你的 draw_ui_text 现在接受的是 entt::id_type (即 font_id_)
    context_.get_renderer().draw_ui_text(camera, text_, font_id_, font_size_, get_screen_position(), text_color_);

    // 渲染子元素（调用基类方法）
    UIElement::render(context);
}

void UILabel::set_text(std::string_view text) {
    text_ = text;
    update_size();
}

void UILabel::set_font_path(std::string_view font_path) {
    font_path_ = font_path;
    font_id_ = entt::hashed_string{font_path.data()}.value();
    update_size();
}

void UILabel::set_font_size(int font_size) {
    font_size_ = font_size;
    update_size();
}

void UILabel::set_text_color(sf::Color text_color) {
    text_color_ = std::move(text_color);
}

void UILabel::update_size() {
    if (text_.empty() || font_path_.empty()) {
        size_ = {0.f, 0.f};
        return;
    }
    
    // 完全保留你原有的尺寸计算逻辑，只是改用 font_path_
    auto* font = context_.get_resource_manager().get_font(entt::hashed_string(font_path_.c_str()));
    sf::Text text_temp(*font, sf::String::fromUtf8(text_.begin(), text_.end()));
    text_temp.setCharacterSize(font_size_);
    size_ = text_temp.getGlobalBounds().size;
}

} // namespace engine::ui