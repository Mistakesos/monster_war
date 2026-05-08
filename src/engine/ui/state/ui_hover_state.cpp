#include "engine/ui/state/ui_hover_state.hpp"
#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/ui/state/ui_pressed_state.hpp"
#include "engine/ui/ui_interactive.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace engine::ui::state {
UIHoverState::UIHoverState(engine::ui::UIInteractive* owner)
    : UIState{owner} {
    owner_->get_context().get_input_manager().on_action(Action::MouseLeft).connect<&UIHoverState::on_mouse_pressed>(this);

    owner_->set_current_sprite("hover"_hs);
    owner_->hover_enter();
    spdlog::debug("切换到悬停状态");
}

UIHoverState::~UIHoverState() {
    owner_->get_context().get_input_manager().on_action(Action::MouseLeft).disconnect<&UIHoverState::on_mouse_pressed>(this);
}

void UIHoverState::update(sf::Time, engine::core::Context& context) {
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_logical_position();
    if (!owner_->is_point_inside(static_cast<sf::Vector2f>(mouse_pos))) {                // 如果鼠标不在UI元素内，则设置正常状态
        owner_->hover_leave();
        owner_->set_next_state(std::make_unique<UINormalState>(owner_));
    }
}

bool UIHoverState::on_mouse_pressed() {
    owner_->set_next_state(std::make_unique<UIPressedState>(owner_));
    return true;
}

} // namespace engine::ui::state