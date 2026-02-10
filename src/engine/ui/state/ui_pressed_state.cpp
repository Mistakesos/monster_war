#include "engine/ui/state/ui_normal_state.hpp"
#include "engine/ui/state/ui_hover_state.hpp"
#include "engine/ui/ui_interactive.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/core/context.hpp"
#include "engine/audio/audio_player.hpp"
#include <spdlog/spdlog.h>
#include "engine/ui/state/ui_pressed_state.hpp"

namespace engine::ui::state {
UIPressedState::UIPressedState(engine::ui::UIInteractive* owner)
    : UIState{owner} {
    owner_->set_sprite("pressed");
    owner_->play_sound("pressed");
    spdlog::debug("切换到按下状态");
}

void UIPressedState::handle_input(engine::core::Context& context) {
    auto& input_manager = context.get_input_manager();
    auto mouse_pos = input_manager.get_mouse_logical_position();
    if (input_manager.is_action_released(Action::MouseLeft)) {
        if (!owner_->is_point_inside(static_cast<sf::Vector2f>(mouse_pos))) { // 松开鼠标时，如果不在UI元素内，则切换到正常状态
            transition<engine::ui::state::UINormalState>();
        } else { // 松开鼠标时，如果还在UI元素内，则触发点击事件
            owner_->clicked();
            transition<engine::ui::state::UIHoverState>();
        }
    }
}
} // namespace engine::ui::state