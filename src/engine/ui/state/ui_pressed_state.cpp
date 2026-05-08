#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/ui/state/ui_hover_state.hpp"
#include "engine/ui/ui_interactive.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/ui/state/ui_pressed_state.hpp"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace engine::ui::state {
UIPressedState::UIPressedState(engine::ui::UIInteractive* owner)
    : UIState{owner} {
    owner_->get_context().get_input_manager().on_action(Action::MouseLeft, engine::input::ActionState::Released).connect<&UIPressedState::on_mouse_released>(this);

    owner_->set_current_sprite("pressed"_hs);
    owner_->play_sound("ui_click"_hs);
    spdlog::debug("切换到按下状态");
}

UIPressedState::~UIPressedState() {
    owner_->get_context().get_input_manager().on_action(Action::MouseLeft, engine::input::ActionState::Released).disconnect<&UIPressedState::on_mouse_released>(this);
}

bool UIPressedState::on_mouse_released() {
    auto& input_manager = owner_->get_context().get_input_manager();
    auto mouse_pos = input_manager.get_mouse_logical_position();
    if (owner_->is_point_inside(static_cast<sf::Vector2f>(mouse_pos))) {
        owner_->set_next_state(std::make_unique<UIHoverState>(owner_));
        owner_->clicked();
    } else {
        owner_->set_next_state(std::make_unique<UINormalState>(owner_));
    }
    return true;
}

} // namespace engine::ui::state