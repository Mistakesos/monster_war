#include "engine/ui/ui_button.hpp"
#include "engine/ui/state/ui_normal_state.hpp"
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui {
UIButton::UIButton(engine::core::Context& context,
                   sf::Sprite normal_sprite, 
                   sf::Sprite hover_sprite, 
                   sf::Sprite pressed_sprite, 
                   sf::Vector2f position, 
                   sf::Vector2f size, 
                   std::function<void()> click_callback,
                   std::function<void()> hover_enter_callback,
                   std::function<void()> hover_leave_callback)
    : UIInteractive{context
    , std::move(position)
    , std::move(size)}
                 , click_callback_{std::move(click_callback)}
                 , hover_enter_callback_{std::move(hover_enter_callback)}
                 , hover_leave_callback_{std::move(hover_leave_callback)} {
    // 注意正常、悬浮、按下都有默认的键名称，如果需要替换的话则覆盖该键下的值
    add_sprite("normal"_hs, normal_sprite);
    add_sprite("hover"_hs, hover_sprite);
    add_sprite("pressed"_hs, pressed_sprite);

    // 设置默认状态为"normal"
    set_current_state(std::make_unique<engine::ui::state::UINormalState>(this));

    spdlog::trace("UIButton 构造完成");
}

} // namespace engine::ui

