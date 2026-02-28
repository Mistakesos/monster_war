#include "engine/ui/ui_button.hpp"
#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/core/context.hpp"
#include "engine/resource/resource_manager.hpp"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

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
    auto tex_normal = resource_manager.get_texture(entt::hashed_string{normal_sprite_id.data(), normal_sprite_id.size()});
    auto tex_hover = resource_manager.get_texture(entt::hashed_string{hover_sprite_id.data(), hover_sprite_id.size()});
    auto tex_pressed = resource_manager.get_texture(entt::hashed_string{pressed_sprite_id.data(), pressed_sprite_id.size()});
    add_sprite("normal"_hs, std::make_unique<sf::Sprite>(*tex_normal));
    add_sprite("hover"_hs, std::make_unique<sf::Sprite>(*tex_hover));
    add_sprite("pressed"_hs, std::make_unique<sf::Sprite>(*tex_pressed));

    // 设置默认状态为"normal"
    set_state(std::make_unique<engine::ui::state::UINormalState>(this));

    // 设置默认音效
    add_sound("hover"_hs, "assets/audio/button_hover.wav"_hs);
    add_sound("pressed"_hs, "assets/audio/button_click.wav"_hs);
    spdlog::trace("UIButton 构造完成");
}

void UIButton::clicked() {
    if (callback_) {
        callback_();
    }
}
} // namespace engine::ui