#include "engine/ui/state/ui_hover_state.hpp"
#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/ui/state/ui_pressed_state.hpp"
#include "engine/ui/ui_interactive.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui::state {
UIHoverState::UIHoverState(engine::ui::UIInteractive* owner)
    : UIState{owner} {
    owner_->set_sprite("hover");
    spdlog::debug("切换到悬停状态");
}

void UIHoverState::handle_input(engine::core::Context& context)
{
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_logical_position();
    if (!owner_->is_point_inside(static_cast<sf::Vector2f>(mouse_pos))) {                // 如果鼠标不在UI元素内，则返回正常状态
        transition<UINormalState>();
    }
    if (input_manager.is_action_pressed(Action::MouseLeftClick)) {  // 如果鼠标按下，则返回按下状态
        transition<UIPressedState>();
    }
}
} // namespace engine::ui::state