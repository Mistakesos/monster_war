#include "engine/ui/ui_button.hpp"
#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/core/context.hpp"
#include "engine/resource/resource_manager.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {
UIButton::UIButton(engine::core::Context& context
                 , std::string_view normal_sprite_id
                 , std::string_view hover_sprite_id
                 , std::string_view pressed_sprite_id
                 , sf::Vector2f position
                 , sf::Vector2f size
                 , std::function<void()> callback)
    : UIInteractive{context
                  , std::move(position)
                  , std::move(size)}
    , callback_{std::move(callback)} {
    auto& resource_manager = context.get_resource_manager();
    auto tex_normal = resource_manager.get_texture(normal_sprite_id);
    auto tex_hover = resource_manager.get_texture(hover_sprite_id);
    auto tex_pressed = resource_manager.get_texture(pressed_sprite_id);
    add_sprite("normal", std::make_unique<sf::Sprite>(*tex_normal));
    add_sprite("hover", std::make_unique<sf::Sprite>(*tex_hover));
    add_sprite("pressed", std::make_unique<sf::Sprite>(*tex_pressed));

    // 设置默认状态为"normal"
    set_state(std::make_unique<engine::ui::state::UINormalState>(this));

    // 设置默认音效
    add_sound("hover", "assets/audio/button_hover.wav");
    add_sound("pressed", "assets/audio/button_click.wav");
    spdlog::trace("UIButton 构造完成");
}

void UIButton::clicked() {
    if (callback_) {
        callback_();
    }
}
} // namespace engine::ui